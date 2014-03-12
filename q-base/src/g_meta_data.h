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
g_mmap_meta_data(
	       char *docroot,

	       char **ptr_x_gpu_reg_X,
	       size_t *ptr_x_gpu_reg_nX,
	       GPU_REG_TYPE **ptr_x_gpu_reg,
	       int *ptr_x_n_gpu_reg

	       )
;
//----------------------------
extern void
g_unmap_meta_data(
		char *x_gpu_reg_X,
		size_t x_gpu_reg_nX

		)
;
//----------------------------
extern int
g_init_meta_data(
	       char *docroot
	       )
;
//----------------------------
