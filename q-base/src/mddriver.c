/* MDDRIVER.C - test driver for MD2, MD4 and MD5
 */

/* Copyright (C) 1990-2, RSA Data Security, Inc. Created 1990. All
   rights reserved.

   RSA Data Security, Inc. makes no representations concerning either
   the merchantability of this software or the suitability of this
   software for any particular purpose. It is provided "as is"
   without express or implied warranty of any kind.

   These notices must be retained in any copies of any part of this
   documentation and/or software.
*/

#include "macros.h"
#define MD 5 /* MODIFIED BY RAMESH SUBRAMONIAN */
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "md5global.h"
#if MD == 2
#include "md2.h"
#endif
#if MD == 4

#include "md4.h"
#endif
#if MD == 5
#include "md5c.h"
#endif

/* DO NOT AUTO GENERATE HEADER FILE  */
/* Length of test block, number of test blocks.
 */
#define TEST_BLOCK_LEN 1000
#define TEST_BLOCK_COUNT 1000


static void my_MDPrint (
			unsigned char digest[LEN_MD5_DIGEST],
			char *out_string
			);


#if MD == 2
#define MD_CTX MD2_CTX
#define MDInit MD2Init
#define MDUpdate MD2Update
#define MDFinal MD2Final
#endif
#if MD == 4
#define MD_CTX MD4_CTX
#define MDInit MD4Init
#define MDUpdate MD4Update
#define MDFinal MD4Final
#endif
#if MD == 5
#define MD_CTX MD5_CTX
#define MDInit MD5Init
#define MDUpdate MD5Update
#define MDFinal MD5Final
#endif

/* Main driver.

Arguments (may be any combination):
-sstring - digests string
-t       - runs time trial
-x       - runs test script
filename - digests file
(none)   - digests standard input
*/
#undef MAIN
#ifdef MAIN
static void MDString PROTO_LIST ((char *));
static void MDTimeTrial PROTO_LIST ((void));
static void MDTestSuite PROTO_LIST ((void));
static int  MDFile PROTO_LIST ((char *));
static void MDFilter PROTO_LIST ((void));
static void MDPrint PROTO_LIST ((unsigned char [LEN_MD5_DIGEST]));

int main (
	int argc,
	char **argv
)
{
  int status = 0;
  int i;

  if (argc > 1) {
    for (i = 1; i < argc; i++)
      if (argv[i][0] == '-' && argv[i][1] == 's') {
	MDString (argv[i] + 2);
}
      else if (strcmp (argv[i], "-t") == 0) {
	MDTimeTrial ();
}j
      else if (strcmp (argv[i], "-x") == 0) {
	MDTestSuite ();
}
      else {
	status = MDFile (argv[i]);
 	conditional_BYE(status);
}
  }
  else {
    MDFilter ();
  }

BYE:
  return(status);
}

/* Measures the time to digest TEST_BLOCK_COUNT TEST_BLOCK_LEN-byte
   blocks.
*/
static void MDTimeTrial ()
{
  MD_CTX context;
  time_t endTime, startTime;
  unsigned char block[TEST_BLOCK_LEN], digest[LEN_MD5_DIGEST];
  unsigned int i;

  printf
    ("MD%d time trial. Digesting %d %d-byte blocks ...", MD,
     TEST_BLOCK_LEN, TEST_BLOCK_COUNT);

  /* Initialize block */
  for (i = 0; i < TEST_BLOCK_LEN; i++)
    block[i] = (unsigned char)(i & 0xff);

  /* Start timer */
  time (&startTime);

  /* Digest blocks */
  MDInit (&context);
  for (i = 0; i < TEST_BLOCK_COUNT; i++)
    MDUpdate (&context, block, TEST_BLOCK_LEN);
  MDFinal (digest, &context);

  /* Stop timer */
  time (&endTime);

  printf (" done\n");
  printf ("Digest = ");
  MDPrint (digest);
  printf ("\nTime = %ld seconds\n", (long)(endTime-startTime));
  printf
    ("Speed = %ld bytes/second\n",
     (long)TEST_BLOCK_LEN * (long)TEST_BLOCK_COUNT/(endTime-startTime));
}

/* Digests a reference suite of strings and prints the results.
 */
static void MDTestSuite ()
{
  printf ("MD%d test suite:\n", MD);

  MDString ("");
  MDString ("a");
  MDString ("abc");
  MDString ("message digest");
  MDString ("abcdefghijklmnopqrstuvwxyz");
  MDString
    ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
  MDString
    ("1234567890123456789012345678901234567890\
1234567890123456789012345678901234567890");
}

/* Digests a file and prints the result.

*/
static int
MDFile (
	char *filename
)
{
  int status = 0;
  FILE *fp;
  MD_CTX context;
  int len;
  unsigned char buffer[1024], digest[LEN_MD5_DIGEST];

  fp = fopen(filename, "rb");
  return_if_fopen_failed(fp, filename, "rb");
  MDInit (&context);
/* ORIGINAL. Modification below
  while (len = fread (buffer, 1, 1024, fp))
    MDUpdate (&context, buffer, len);
*/
  for ( ; ; ) {
    len = fread (buffer, 1, 1024, fp);
    if ( len < 0 ) { break; }
    MDUpdate (&context, buffer, len);
  }
  MDFinal (digest, &context);
  fclose_if_non_null(fp);

  printf ("MD%d (%s) = ", MD, filename);
  MDPrint (digest);
  printf ("\n");
BYE:
  fclose_if_non_null(fp);
  return(status);
}

/* Digests the standard input and prints the result.
 */
static void MDFilter ()
{
  MD_CTX context;
  int len;
  unsigned char buffer[LEN_MD5_DIGEST], digest[LEN_MD5_DIGEST];

  MDInit (&context);
  for ( ; ; ) {
    len = fread (buffer, 1, LEN_MD5_DIGEST, stdin);
    if ( len < 0 ) { break; }
    MDUpdate (&context, buffer, len);
  }
  MDFinal (digest, &context);

  MDPrint (digest);
  printf ("\n");
}

/* Digests a string and prints the result.
 */
static void MDString (string)
     char *string;
{
  MD_CTX context;
  unsigned char digest[LEN_MD5_DIGEST];
  unsigned int len = strlen (string);

  MDInit (&context);
  MDUpdate (&context, string, len);
  MDFinal (digest, &context);

  printf ("MD%d (\"%s\") = ", MD, string);
  MDPrint (digest);
  printf ("\n");
}

/* Prints a message digest in hexadecimal.
 */
static void MDPrint (digest)
     unsigned char digest[LEN_MD5_DIGEST];
{

  unsigned int i;

  for (i = 0; i < LEN_MD5_DIGEST; i++)
    printf ("%02x", digest[i]);
}

#endif

/* Digests a string and prints the result.
 */
void my_MDString (
		  const char *in_string,
		  char *out_string
		  )
{
  MD_CTX context;
  unsigned char digest[LEN_MD5_DIGEST];
  unsigned int len = strlen (in_string);

  MDInit (&context);
  MDUpdate (&context, in_string, len);
  MDFinal (digest, &context);
  my_MDPrint (digest, out_string);
}
static void my_MDPrint (
			unsigned char digest[LEN_MD5_DIGEST],
			char *out_string
			)
{

  unsigned int i, j;
  char stemp[4];

  for (i = 0; i < LEN_MD5_DIGEST; i++) {
    for ( j = 0; j < 4; j++ ) {
      stemp[j] = '\0';
    }
    sprintf (stemp, "%02x", digest[i]);
    strcat(out_string, stemp);
  }
}
