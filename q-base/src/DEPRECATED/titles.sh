#!/bin/sh
LOC=$HOME/WORK/TITLE/src
DB=db1.sq3
export Q_DOCROOT=$PWD/$DB
t1=twdict # title word dictonary
sqlite3 $DB < docroot.sql
#-- Create a table for the title word dictionary
q add_tbl "" $t1  0
if [ $? != 0 ]; then echo FAILURE; exit 1; fi
#-- Add the title word as a primary field 
q add_fld "" $t1 tword "fldtype=wchar_t:sz=48" $LOC/_xxx.word
if [ $? != 0 ]; then echo FAILURE; exit 1; fi
#-- Add the hash of the title word as an auciliary field
q add_aux_fld "" $t1 tword twhash $LOC/_xxx.hash hash
if [ $? != 0 ]; then echo FAILURE; exit 1; fi
#-- Create a table for the members/titles
t2=member
q add_tbl "" $t2 0
if [ $? != 0 ]; then echo FAILURE; exit 1; fi
#-- Add the mid field for this table
q add_fld "" $t2 mid "fldtype=int:sz=4" $LOC/_xxx.mid
if [ $? != 0 ]; then echo FAILURE; exit 1; fi
#-- Add the chron (which position) field for this table
q add_fld "" $t2 chron "fldtype=int:sz=4" $LOC/_xxx.chron
if [ $? != 0 ]; then echo FAILURE; exit 1; fi
#-- Add the title - variable length field.
#-- Note that we are actually adding the hash of the words 
#-- in the title and not the title itself
q add_fld "" $t2 twhash "fldtype=int:sz=0" $LOC/_xxx.hw
if [ $? != 0 ]; then echo FAILURE; exit 1; fi
#-- Since twhash is variable length field, we need to provide length
q add_aux_fld "" $t2 twhash twhash.len $LOC/_xxx.nw sz
if [ $? != 0 ]; then echo FAILURE; exit 1; fi
# Add first character of each word in title as variable length field
q add_fld "" $t2 fc "fldtype=wchar_t:sz=0" $LOC/_xxx.fc
if [ $? != 0 ]; then echo FAILURE; exit 1; fi
#-- Since fc is variable length field, we need to provide length
cp $LOC/_xxx.nw $LOC/_fc.sz
q add_aux_fld "" $t2 fc fc.len $LOC/_xxx.fc.sz sz
if [ $? != 0 ]; then echo FAILURE; exit 1; fi
