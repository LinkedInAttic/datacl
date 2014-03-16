#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

#include <sys/time.h>

#include "q_types.hpp"
#include "q_url.hpp"
#include "q_gpu.hpp"

#include "q_gpu_registers.hpp"

#define SERVER_REQ_Q 16 // number of concurrent requests

using std::ostringstream;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;

/**********************************************************************/
/* Get a line from a socket, whether the line ends in a newline,
 * carriage return, a CRLF combination or none of these (i.e. end of data).
 * strips out the NL, CR, CRLF
 * returns false if failed to read any data from the socket
 * Parameters: the socket descriptor
 *             the string to save the data in
 * Returns: true if any data was read, false otherwise
 **********************************************************************/

bool
get_line(
         int source, // socket
         string & line
         ) {
  line.clear();
  char c;
  if (recv(source, &c, 1, MSG_PEEK) <= 0) {
    return false;
  } else {
    while (recv(source, &c, 1, 0) == 1) {
      if (c == '\r') {
        if ((recv(source, &c, 1, MSG_PEEK) == 1) && (c == '\n')) {
          recv(source, &c, 1, 0); // remove the newline
        }
        break;
      } else if (c == '\n') {
        break;
      }
      line.push_back(c);
    }
  }
  return true;
}

void
send_msg(int dest, // socket
         string const & hdr,
         string const & msg) {
  static string const srv("Server: qhttpd/0.1.0\r\n");
  send(dest, hdr.c_str(), hdr.size(), 0);
  send(dest, srv.c_str(), srv.size(), 0);
  send(dest, msg.c_str(), msg.size(), 0);
}

/* Invocations
   curl localhost:8000/init?TABLESPACE=test1
   TOKEN=12345
   curl localhost:8000/free?TOKEN=$TOKEN
   instr="q f1s1opf2 t1 f1 f2 '&&' f3"
   echo $instr
   q f1s1opf2 t1 f1 f2 '&&' f3
   codestr=`qtils urlencode "$instr"`
   echo $codestr
   q+f1s1opf2+t1+f1+f2+%27%26%26%27+f3
   curl --url "localhost:8000/q?TOKEN=$TOKEN&COMMAND=$codestr"

   *
   *
   *
   */

class GPU_timer {
public:
  GPU_timer(string const & nm)
    : name(nm),
      start(),
      end() {
    gettimeofday(&start, NULL);
  }
  ~GPU_timer() {
    gettimeofday(&end, NULL);
    double diff = ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec));
    cout << "Time Taken for " << name << ": " << (diff * 1e-3) << " msec" << endl;
  }

private:
  string name;
  struct timeval start, end;
};

int
process_get(
            string const & url,
            int client
            ) {

  string const prefix("/gq?COMMAND=");
  string found_prefix = url.substr(0, prefix.size());
  if (strncasecmp(prefix.c_str(), found_prefix.c_str(), prefix.size()) != 0) {
    static string const bad_req_hdr("HTTP/1.1 400 Bad Request\r\n");
    static string const bad_req_msg("Content-type: text/html\r\n\r\n"
                                    "<HTML><TITLE>Bad Request</TITLE>\n"
                                    "<BODY><P>The Q GPU server could not interpret your request</P>"
                                    "</BODY></HTML>\r\n");
    cerr << "ERROR: expected request starting with " << prefix << " but found " << url << endl;
    send_msg(client, bad_req_hdr, bad_req_msg);
    QDB::whereami(__FILE__, __LINE__);
    return QDB::Q_FAIL;
  }

  string query;
  if (QDB::url_decode(url.substr(prefix.size()).c_str(), query) != QDB::Q_PASS) {
    QDB::whereami(__FILE__, __LINE__);
    return QDB::Q_FAIL;
  }

  //  cerr << "DEBUG: START \"" << query << "\"" << endl;

  vector<string> args;
  boost::char_separator<char> sep(", \t\n");
  boost::tokenizer<boost::char_separator<char> > tok(query, sep);
  for (auto i = tok.begin(), iEnd = tok.end(); i != iEnd; ++i) {
    args.push_back(*i);
  }

  int status;
  string result;
  {
    // GPU_timer timer(args[1]);
    GPU_timer timer(query);
    status = QDB::qgpu(args, result);
  }

  if (status != QDB::Q_PASS) {
    static string const hdr("HTTP/1.1 500 Internal Server Error\r\n");
    string msg(string("{ \"msg\" : \"QGPU server failed to execute: ") +
               query + string("\" }\r\n"));
    string full_msg = string("Content-type: application/json\r\n"
                             "Content-Length: ") + boost::lexical_cast<string>(msg.size()) + string("\r\n\r\n") + msg;
    send_msg(client, hdr, full_msg);
  } else {
    static string const hdr("HTTP/1.1 200 OK\r\n");
    string msg(string("{ \"msg\" : \"") +
               result + string("\" }\r\n"));
    string full_msg = string("Content-type: application/json\r\n"
                             "Content-Length: ") + boost::lexical_cast<string>(msg.size()) + string("\r\n\r\n") + msg;
    send_msg(client, hdr, full_msg);
  }

  //  cerr << "DEBUG: DONE \"" << query << "\"" << endl;

  return QDB::Q_PASS;
}

/**********************************************************************/
/* A request has caused a call to accept() on the server port to
 * return.  Process the request appropriately.
 * Parameters: the socket connected to the client */
/**********************************************************************/
int
accept_request(
               void *voidptr
               ) {

  int client = *((int *)(voidptr));
  string request;
  if (!get_line(client, request) || request.empty()) {
    cerr << "ERROR: failed to get request from client" << endl;
    QDB::whereami(__FILE__, __LINE__);
    return QDB::Q_FAIL;
  }

  static string const no_impl_hdr("HTTP/1.1 501 Method Not Implemented\r\n");
  static string const no_impl_msg("Content-type: text/html\r\n\r\n"
                                  "<HTML><HEAD><TITLE>Method Not Implemented</TITLE></HEAD>\n"
                                  "<BODY><P>HTTP request method not supported.</BODY></HTML>\r\n");
  if (strncasecmp(request.substr(0, 4).c_str(), "GET ", 4) != 0) {
    cerr << "ERROR: expected request starting with GET but found " << request << endl;
    send_msg(client, no_impl_hdr, no_impl_msg);
    QDB::whereami(__FILE__, __LINE__);
    return QDB::Q_FAIL;
  }
  request = request.substr(request.find_first_not_of(' ', 4));
  request = request.substr(0, request.find(" HTTP/1.1"));
  if (request.empty()) {
    cerr << "ERROR: got empty request" << endl;
    send_msg(client, no_impl_hdr, no_impl_msg);
    QDB::whereami(__FILE__, __LINE__);
    return QDB::Q_FAIL;
  }

  int status = process_get(request, client);
  if ( status < 0 ) {
    static string const hdr("HTTP/1.1 500 Internal Server Error\r\n");
    static string const msg("\r\nUNIDENTIFIED ERROR\r\n");
    cerr << "ERROR: unable to process request " << request << " for unknown reason" << endl;
    send_msg(client, hdr, msg);
  }

  close(client);

  return QDB::Q_PASS;
}

/**********************************************************************/
/* This function starts the process of listening for web connections
 * on a specified port.  If the port is 0, then dynamically allocate a
 * port and modify the original port variable to reflect the actual
 * port.
 * Parameters: pointer to variable containing the port to connect on
 * Returns: the socket */
/**********************************************************************/
int
startup(
        uint16_t port
        ) {
  int httpd = socket(PF_INET, SOCK_STREAM, 0);
  if (httpd == -1) {
    perror("socket");
    exit(1);
  }

  struct sockaddr_in name;
  memset(&name, 0, sizeof(name));
  name.sin_family = AF_INET;
  name.sin_port = htons(port);
  name.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0) {
    perror("bind");
    exit(1);
  }
  if (listen(httpd, SERVER_REQ_Q) < 0) {
    perror("listen");
    exit(1);
  }
  return(httpd);
}


/**********************************************************************/

int
main(
     int argc,
     char ** argv
     ) {
  if ( argc != 1 ) {
    QDB::whereami(__FILE__, __LINE__);
    return QDB::Q_FAIL;
  }

  uint16_t port = QDB::GPU::getInstance().gpu_port;
  int server_sock = startup(port);
  cout << "httpd running on port " << port << " in single thread mode" << endl;

  while (1) {
    struct sockaddr_in client_name;
    socklen_t client_name_len = sizeof(client_name);
    int client_sock = accept(server_sock,
                             (struct sockaddr *)&client_name,
                             &client_name_len);
    int flag=1;
    int sz = sizeof(int);
    setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sz);
    if (client_sock == -1) {
      perror("accept");
      return QDB::Q_FAIL;
    }
    accept_request(&client_sock);
  }

  close(server_sock);
  return QDB::Q_PASS;
}
