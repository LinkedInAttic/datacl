#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#include "q_url.hpp"
#include "q_types.hpp"

static inline
char
from_hex(
         char ch
         ) {
  return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/* Converts an integer value to its hex character*/
static inline
char
to_hex(
       char code
       ) {
  static char hex[] = "0123456789abcdef";
  return hex[code & 0xFF];
}


namespace QDB {
  int
  url_encode(
             char const * in_str,
             std::string & out_str
             ) {
    out_str = std::string("");
    while (*in_str) {
      if (isalnum(*in_str) || *in_str == '-' || *in_str == '_' || *in_str == '.' || *in_str == '~') {
        out_str.push_back(*in_str);
      } else if (*in_str == ' ') {
        out_str.push_back('+');
      } else {
        out_str.push_back('%');
        out_str.push_back(to_hex(*in_str >> 4));
        out_str.push_back(to_hex(*in_str & 0xFF));
      }
      ++in_str;
    }
    return QDB::Q_PASS;
  }

  /* Returns a url-decoded version of str */
  int
  url_decode(
             char const * in_str,
             std::string & out_str
             ) {
    out_str = std::string("");
    if ( !in_str || ( *in_str == '\0' ) ) {
      whereami(__FILE__, __LINE__);
      return QDB::Q_FAIL;
    }

    while (*in_str) {
      if (*in_str == '%') {
        if (in_str[1] && in_str[2]) {
          out_str.push_back(from_hex(in_str[1]) << 4 | from_hex(in_str[2]));
          in_str += 2;
        }
      } else if (*in_str == '+') {
        out_str.push_back(' ');
      } else {
        out_str.push_back(*in_str);
      }
      ++in_str;
    }
    return QDB::Q_PASS;
  }

};


extern "C" {

  int
  url_encode(
             char *in_str,
             char **ptr_out_str
             ) {
    std::string result;
    if (QDB::url_encode(in_str, result) != QDB::Q_PASS) {
      return QDB::Q_FAIL;
    }
    *ptr_out_str = (char *)malloc(sizeof(char) * (result.size() + 1));
    strcpy(*ptr_out_str, result.c_str());
    return QDB::Q_PASS;
  }

  int url_decode(
                 char *in_str,
                 char **ptr_out_str
                 ) {
    std::string result;
    if (QDB::url_decode(in_str, result) != QDB::Q_PASS) {
      return QDB::Q_FAIL;
    }
    *ptr_out_str = (char *)malloc(sizeof(char) * (result.size() + 1));
    strcpy(*ptr_out_str, result.c_str());
    return QDB::Q_PASS;
  }

  int
  ext_url_encode(
                 char *in_str,
                 char *rslt_buf,
                 int sz_rslt_buf
                 ) {
    std::string result;
    if ((QDB::url_encode(in_str, result) != QDB::Q_PASS) ||
        (int(result.size() + 1) > sz_rslt_buf)) {
      return QDB::Q_FAIL;
    }
    strcpy(rslt_buf, result.c_str());
    return QDB::Q_PASS;
  }

  int
  ext_url_decode(
                 char *in_str,
                 char *rslt_buf,
                 int sz_rslt_buf
                 ) {
    std::string result;
    if ((QDB::url_decode(in_str, result) != QDB::Q_PASS) ||
        (int(result.size() + 1) > sz_rslt_buf)) {
      return QDB::Q_FAIL;
    }
    strcpy(rslt_buf, result.c_str());
    return QDB::Q_PASS;
  }

}
