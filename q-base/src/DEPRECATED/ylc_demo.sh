#!/usr/local/bin/bash
set -e 
# shopt -s expand_aliases
# alias ylc="valgrind $valgrind_options ylc"

# Making sure old fields still exist

ylc init 
ylc add xxx TB
ylc list_lists
ylc push xxx bis_recruiter
ylc push xxx bis_hr
ylc push xxx b1
ylc push xxx b2
ylc limit xxx 10000:20000:-1:100000000
ylc limit xxx 200000000:-1:10:-1
ylc exclude xxx b5
ylc exclude xxx b4
ylc exclude xxx b3
