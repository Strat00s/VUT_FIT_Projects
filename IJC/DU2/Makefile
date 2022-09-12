CFLAGS = -O2 -std=c99 -pedantic -Wall -Wextra -fPIC
CC = gcc

_all: wordcount wordcount-dynamic tail

.PHONY: wordcount wordcount-dynamic tail zip clean

########################################################################
tail:
	$(CC) $(CFLAGS) -o tail tail.c

wordcount: wordcount.o libhtab.a
	$(CC) $(CFLAGS) -o wordcount -static wordcount.o -L. -lhtab

wordcount-dynamic: wordcount.o libhtab.so
	$(CC) $(CFLAGS) -o wordcount-dynamic wordcount.o -L. -lhtab

########################################################################

libhtab.a: wordcount.o htab_begin.o htab_bucket_count.o htab_clear.o htab_end.o htab_erase.o htab_find.o htab_free.o htab_hash_fun.o htab_init.o htab_iterator_get_key.o htab_iterator_get_value.o htab_iterator_next.o htab_iterator_set_value.o htab_lookup_add.o htab_size.o io.o iterator_init.o
	ar crs libhtab.a $^

libhtab.so: wordcount.o htab_begin.o htab_bucket_count.o htab_clear.o htab_end.o htab_erase.o htab_find.o htab_free.o htab_hash_fun.o htab_init.o htab_iterator_get_key.o htab_iterator_get_value.o htab_iterator_next.o htab_iterator_set_value.o htab_lookup_add.o htab_size.o io.o iterator_init.o
	$(CC) $(CFLAGS) -shared -o libhtab.so $^

########################################################################

wordcount.o: wordcount.c
	$(CC) $(CFLAGS) -c $^


htab_begin.o: htab_begin.c
	$(CC) $(CFLAGS) -c $^

htab_bucket_count.o: htab_bucket_count.c
	$(CC) $(CFLAGS) -c $^

htab_clear.o: htab_clear.c
	$(CC) $(CFLAGS) -c $^

htab_end.o: htab_end.c
	$(CC) $(CFLAGS) -c $^

htab_erase.o: htab_erase.c
	$(CC) $(CFLAGS) -c $^

htab_find.o: htab_find.c
	$(CC) $(CFLAGS) -c $^

htab_free.o: htab_free.c
	$(CC) $(CFLAGS) -c $^

htab_hash_fun.o: htab_hash_fun.c
	$(CC) $(CFLAGS) -c $^

htab_init.o: htab_init.c
	$(CC) $(CFLAGS) -c $^

htab_iterator_get_key.o: htab_iterator_get_key.c
	$(CC) $(CFLAGS) -c $^

htab_iterator_get_value.o: htab_iterator_get_value.c
	$(CC) $(CFLAGS) -c $^

htab_iterator_next.o: htab_iterator_next.c
	$(CC) $(CFLAGS) -c $^

htab_iterator_set_value.o: htab_iterator_set_value.c
	$(CC) $(CFLAGS) -c $^

htab_lookup_add.o: htab_lookup_add.c
	$(CC) $(CFLAGS) -c $^

htab_size.o: htab_size.c
	$(CC) $(CFLAGS) -c $^

io.o: io.c
	$(CC) $(CFLAGS) -c $^

iterator_init.o: iterator_init.c
	$(CC) $(CFLAGS) -c $^

########################################################################

zip:
	zip xbasty00.zip *.c *.h Makefile
clean:
	rm -f *.o *.zip	tail wordcount wordcount-dynamic libhtab.a libhtab.so
