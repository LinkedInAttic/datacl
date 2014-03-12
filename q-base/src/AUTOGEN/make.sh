#!/bin/sh
gcc -g  -ansi -pedantic -pthread -std=gnu99 -Wall  \
	 sat_vec_f1f2opf3.c vec_f1f2opf3.c \
	 get_sz_op_fld.c chunking.c \
	 ../auxil.c ../fsize.c ../sort.c ../mk_file.c \
	 assign_char.c \
	 A0000_f1f2opf3_arith.c \
	 A0001_f1f2opf3_arith.c \
	 A0002_f1f2opf3_arith.c \
	 A0003_f1f2opf3_arith.c \
	 A0004_f1f2opf3_arith.c \
	 A0005_f1f2opf3_arith.c \
	 A0006_f1f2opf3_arith.c \
	 A0007_f1f2opf3_arith.c \
	 A0008_f1f2opf3_arith.c \
	 A0009_f1f2opf3_arith.c \
	 A0010_f1f2opf3_arith.c \
	 A0011_f1f2opf3_arith.c \
	 A0012_f1f2opf3_arith.c \
	 A0013_f1f2opf3_arith.c \
	 A0015_f1f2opf3_arith.c \
	 A0016_f1f2opf3_arith.c \
	 B0000_f1f2opf3_cond.c \
	 B0001_f1f2opf3_cond.c \
	 B0002_f1f2opf3_cond.c \
	 B0003_f1f2opf3_cond.c \
	 B0004_f1f2opf3_cond.c \
	 B0005_f1f2opf3_cond.c \
	 -I.. 
