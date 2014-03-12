/*
© [2013] LinkedIn Corp. All rights reserved.
Licensed under the Apache License, Version 2.0 (the "License"); you may
not use this file except in compliance with the License. You may obtain
a copy of the License at  http://www.apache.org/licenses/LICENSE-2.0
 
Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an "AS IS"
BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied.
*/
extern void split_query_str(
		     char *src, 
		     char *dest
		     )
;
//----------------------------
extern void send_server_info(
		      int client
		      )
;
//----------------------------
extern void acquire_namespace(
		       int ns_map_indx
		       )
;
//----------------------------
extern void release_namespace(
		       int ns_map_indx
		       )
;
//----------------------------
extern void execute_plugin(
		    char *path, 
		    char *query_string, 
		    int client, 
		    int ns_map_indx
		    )
;
//----------------------------
extern int execute_q_query(
		    char *query_string,
		    unsigned int token,
		    int client
		    )
;
//----------------------------
extern int generate_namespace(
		       char *buf
		       )
;
//----------------------------
extern int send_identifier(
		    unsigned int token,
		    int client
		    )
;
//----------------------------
extern int process_get(
		char *url, 
		char *encoded_query_string, 
		int client
		)
;
//----------------------------
extern void *accept_request(
		     void *voidptr
		     )
;
//----------------------------
extern void bad_request(
		 int client
		 )
;
//----------------------------
extern void cat(
	 int client, 
	 FILE *resource
	 )
;
//----------------------------
extern int send_msg(
	     char *msg,
	     int status_code,
	     int client
	     )
;
//----------------------------
extern void error_die(
	       const char *sc
	       )
;
//----------------------------
extern int get_line(
	     int sock, 
	     char *buf, 
	     int size
	     )
;
//----------------------------
extern void headers(
	     int client, 
	     const char *filename
	     )
;
//----------------------------
extern void send_invalid_req(
		      int client
		      )
;
//----------------------------
extern void not_found(
	       int client
	       )
;
//----------------------------
extern void serve_file(
		int client, 
		const char *filename
		)
;
//----------------------------
extern int startup(
	    uint16_t *port
	    )
;
//----------------------------
extern void unimplemented(
		   int client
		   )
;
//----------------------------
