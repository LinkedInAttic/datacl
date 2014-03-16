#ifndef Q_URL_HPP
#define Q_URL_HPP

#ifdef __cplusplus

#include <string>

namespace QDB {
  /* Converts a hex character to its integer value */
  /* Returns a url-encoded version of str */
  int
  url_encode(
             char const * in_str,
             std::string & out_str
             );

  /* Returns a url-decoded version of str */
  int
  url_decode(
             char const * in_str,
             std::string & out_str
             );
};

extern "C" {

  int
  url_encode(
             char *in_str,
             char **ptr_out_str
             );

  int url_decode(
                 char *in_str,
                 char **ptr_out_str
                 );

  int
  ext_url_encode(
                 char *in_str,
                 char *rslt_buf,
                 int sz_rslt_buf
                 );

  int
  ext_url_decode(
                 char *in_str,
                 char *rslt_buf,
                 int sz_rslt_buf
                 );
}

#else

int
url_encode(
           char *in_str,
           char **ptr_out_str
           );

int
url_decode(
           char *in_str,
           char **ptr_out_str
           );

int
ext_url_encode(
               char *in_str,
               char *rslt_buf,
               int sz_rslt_buf
               );

int
ext_url_decode(
               char *in_str,
               char *rslt_buf,
               int sz_rslt_buf
               );

#endif // __cplusplus

#endif // Q_URL_HPP
