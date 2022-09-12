CC = gcc
CFLAGS = -g -O2 -std=c11 -pedantic -Wall -Wextra

all_: steg-decode primes

#build individual files (so that we don't need to compile all of them when just one is changed)
steg-decode-o: steg-decode.c
	$(CC) $(CFLAGS) -c steg-decode.c

ppm-o: ppm.c
	$(CC) $(CFLAGS) -c ppm.c

error-o: error.c
	$(CC) $(CFLAGS) -c error.c

eratosthenes-o: eratosthenes.c
	$(CC) $(CFLAGS) -c eratosthenes.c

primes-o: primes.c
	$(CC) $(CFLAGS) -c primes.c

steg-decode: steg-decode.o ppm.o error.o eratosthenes.o
	$(CC) $(CFLAGS) -o steg-decode steg-decode.o ppm.o error.o eratosthenes.o -lm

primes: primes.o error.o eratosthenes.o
	$(CC) $(CFLAGS) -o primes primes.o error.o eratosthenes.o -lm
	$(CC) $(CFLAGS) -DUSE_INLINE -o primes-i primes.o error.o eratosthenes.o -lm

run: primes
	ulimit -s unlimited; ./primes
	ulimit -s unlimited; ./primes-i

clean:
	rm -f primes primes-i steg-decode *.o