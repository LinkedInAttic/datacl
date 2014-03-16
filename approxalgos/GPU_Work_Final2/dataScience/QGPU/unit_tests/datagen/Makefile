all: datagen_I1 datagen_I4

datagen_I1:
	gcc datagen_I1.c mmap.c new_mk_temp_file.c -o datagen_I1

datagen_I4:
	gcc datagen_I4.c mmap.c new_mk_temp_file.c -o datagen_I4

clean:
	rm -rf datagen_I1 datagen_I4
