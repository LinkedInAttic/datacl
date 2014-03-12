datacl
==========================================

**datacl** is a collection of data-wrangling utilities. While Hadoop has
emerged as the de-facto standard for large data analysis tasks, there are
often a large number of simple data manipulation operations that need to
be performed in interactive mode as a post-processing step. The data for
post-processing is often small enough to fit on a user's desktop.
**datacl** is collection of efficient utilities for a data scientist.

In our experience, the life of a data scientist is often less glamorous
than the public perception. Accelerating the speed of inquiry --- both
in terms of run time and scripting time --- significantly decreases some
of the pains that come with the territory, opening up time to do
higher-level thinking.

While **datacl** was designed primarily as a useful toolkit for data
scientists, we believe it can be used in other contexts as well. A case
in point is our paper entitled "In Data Veritas --- Data Driven Testing for Distributed
Systems", which was presented at DBTest2013, a SIGMOD workshop in New York in 2013.

We believe by open sourcing, we can both contribute to the community
while benefiting from their feedback and contributions.

The name is inspired by Tcl. Ousterhout writes
> The motive behind this model is that standard compiled languages are good for writing low-level algorithms to perform the basic computational tasks required, but tedious for less computational, more descriptive tasks like defining interfaces. Tcl's implementation of the dichotomy, for example, basically involves writing functions in C, exposing them as Tcl procedures using the fairly simple Tcl c API, and then writing scripts to tie the functions together and create a working application out of it."


---

Installation
=====================================

* Copy the file datacl.tgz into a directory and cd to that directory
* tar -zcvf datacl.tgz
* cd src 
* make
* You should get a single file called **q**
* cd ../test/
* You will see a number of examples and unit tests over here. Each sub-directory has a file called *test.sh* which can be executed

You need to set 2 environment variables.
---------------------------------------
* **Q_DOCROOT** This is the directory where the meta data will be stored
* **Q\_DATA\_DIR** This is the directory where the      data will be stored
* **Q\_LOGFILE** Name of log file 

---

Hello World
======================================


    mkdir /tmp/datacl_test
    export Q_DOCROOT=/tmp/datacl_test
    export Q_DATA_DIR=/tmp/datacl_test
    q init # This initializes the meta data
    q add_tbl t1 10 # Creates a table with 10 rows
    # Create a 4 byte integer field f1 in t1 whose values are 1, 2, ...
    q s_to_f t1 f1 'op=[seq]:start=[1]:incr=[1]:fldtype=[I4]'
    q list_tbls # list tables
    q describe t1    # describes table t1
    q describe t1 f1 # describes fld f1 in table t1
    q pr_fld t1 f1 # Prints the values of f1 in t1
    q fop    t1 f1 sortD # Sorts f1 in t1 in descending order
    q pr_fld t1 f1 # Notice that values have switched order
    q delete t1    # Clean up


---

Organization of Code
=====================================
The code is organized as follows

* AUX -- contains (usually) tiny functions where the bulk of compute time should be spent e.g., adding 2 columns to create a third, etc.
* dir_utils -- contains utility functions of generic utility e.g., hashing functions, parsing strings, etc.
* dir_dbutils --- contains utility functions that are specific to datacl * e.g., creating temporary files, etc.
* dir\_ro\_verbs --- contains functions that are *read-only verbs* e.g., the MySQL equivalent of *show tables* or *describe table*
* dir\_ro\_verbs --- contains functions that are *read-only verbs* e.g., the MySQL equivalent of *show tables* or *describe table*
* dir\_wr\_verbs --- contains functions that are *writable verbs* e.g., these cause changes in the data stored e.g., adding 2 columns to create a third is effected by the verb *f1f2opf3*

---

TODO
=====================================
There are several way in which the code can be improved. We list the
most important ones.

* Functional testing in terms of unit tests
* Performance tests and benchmarks
* Reducing the size of the code by using templates
* XXXXX



License
====================================

© [2013] LinkedIn Corp. All rights reserved.
Licensed under the Apache License, Version 2.0 (the "License"); you may
not use this file except in compliance with the License. You may obtain
a copy of the License at  http://www.apache.org/licenses/LICENSE-2.0
 
Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an "AS IS"
BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied.


=======
