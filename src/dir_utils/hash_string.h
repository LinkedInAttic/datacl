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
extern int
int_hash_string(
	    const char *original_in_string,
	    unsigned long long *ptr_hash
	    );
extern int
ext_hash_string(
    char *in_string,
    char *hashtype
    );
extern int
hash_string_UI4(
	    const char *original_in_string,
	    unsigned int *ptr_hash
	    );
extern int
hash_string_UI8(
	    const char *original_in_string,
	    unsigned long long *ptr_hash
	    );
