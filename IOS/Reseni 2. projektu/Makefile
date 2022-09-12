_all: proj2

.PHONY: proj2 zip clean

proj2:
	gcc -std=gnu99 -Wall -Wextra -Werror -pedantic -o proj2 proj2.c -pthread

zip:
	zip proj2.zip proj2.c Makefile
clean:
	rm -f *.o proj2 proj2.zip proj2.out
