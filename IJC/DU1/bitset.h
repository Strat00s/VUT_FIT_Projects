// bitset.h
// Řešení IJC-DU1 (19.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Header file pro práci s bitsety

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>

#include "error.h"

typedef unsigned long bitset_t[];
typedef unsigned long bitset_index_t;

#define SIZEOF_UL (sizeof(unsigned long)*CHAR_BIT)
#define UL_BYTES(size) ((size/SIZEOF_UL) + (size%SIZEOF_UL > 0)+1)	//set the corrent number of bytes for the wanted size.
#define SIZEOF(name) (sizeof(*name)*CHAR_BIT)

/* To get the required size of bits, check our wanted size/(sizeof(unsinged long))*CHAR_BYT
 * 0-0.9999... -> 0 | 1-1.9999... -> 1 and so on. So we have to ask, if there is a reminader. If there is any, alloc 1 more
 * Example: We want 65 bits. Our size will be: 1 (to save the size number) + 1 (65/64 = 1.015625) + 1 (65%64 = 1) = 3*/
#define bitset_create(jmeno_pole, velikost) static_assert(0 < velikost, "velikost pole <= 0"); unsigned long jmeno_pole[UL_BYTES(velikost)]; jmeno_pole[0] = velikost;\
	for(long unsigned int i = 1; i < UL_BYTES(velikost); i++) {jmeno_pole[i] = 0;}

#define bitset_alloc(jmeno_pole, velikost) assert(velikost > 0); unsigned long *jmeno_pole; jmeno_pole = malloc(sizeof(unsigned long *)*UL_BYTES(velikost)); jmeno_pole[0] = velikost;\
	for(long unsigned int i = 1; i < UL_BYTES(velikost); i++) {jmeno_pole[i] = 0;}\
	if(jmeno_pole == NULL){\
		error_exit("bitset_alloc: Chyba alokace paměti");\
	}

//inline functions are practically identical to macros. Way slower than macros with no optimization, a bit faster with -O2 
#ifdef USE_INLINE
static inline unsigned long bitset_size(bitset_t jmeno_pole){
	return jmeno_pole[0];
}

static inline void bitset_setbit(bitset_t jmeno_pole, bitset_index_t index, int vyraz){
	if (index > bitset_size(jmeno_pole)-1){
		error_exit("bitset_setbit: Index %lu mimo rozsah 0..%lu",index, bitset_size(jmeno_pole)-1);
	}
	else if (vyraz){
		jmeno_pole[index/SIZEOF(jmeno_pole)+1] |= ((unsigned long)1 << index%SIZEOF(jmeno_pole));
	}
	else {
		jmeno_pole[index/SIZEOF(jmeno_pole)+1] &= ~((unsigned long)1 << index%SIZEOF(jmeno_pole));
	}
}

static inline int bitset_getbit(bitset_t jmeno_pole, bitset_index_t index){
	if (index > bitset_size(jmeno_pole)-1){
		error_exit("bitset_getbit: Index %lu mimo rozsah 0..%lu",(unsigned long)index, bitset_size(jmeno_pole)-1);
		return -1;
	}
	else {
		return ((jmeno_pole[index/SIZEOF(jmeno_pole)+1] & ((unsigned long)1 << index%SIZEOF(jmeno_pole))) > 0 /*? 1 : 0*/);
	}
}

static inline void bitset_free(bitset_t jmeno_pole){
	if (jmeno_pole != NULL){
		free(jmeno_pole);
	}
}
#else
#define bitset_size(jmeno_pole) jmeno_pole[0]

// similiar to create and alloc. Go to the correct index and then use module to select correct bit (and change it)
#define bitset_setbit(jmeno_pole, index, vyraz)\
	if (index > bitset_size(jmeno_pole)-1){\
		error_exit("bitset_setbit: Index %lu mimo rozsah 0..%lu",(unsigned long)index, bitset_size(jmeno_pole)-1);\
	}\
	else if (vyraz){\
		jmeno_pole[index/SIZEOF(jmeno_pole) +1] |= ((unsigned long)1 << index%SIZEOF(jmeno_pole));\
	}\
	else{\
		jmeno_pole[index/SIZEOF(jmeno_pole) +1] &= ~((unsigned long)1 << index%SIZEOF(jmeno_pole));\
	}

// can't use if
#define bitset_getbit(jmeno_pole, index) (index > bitset_size(jmeno_pole)-1) ?\
	error_exit("bitset_getbit: Index %lu mimo rozsah 0..%lu",(unsigned long)index, bitset_size(jmeno_pole)-1), 0 :\
	((jmeno_pole[index/SIZEOF(jmeno_pole)+1] & ((unsigned long)1 << index%SIZEOF(jmeno_pole))) > 0 /*? 1 : 0*/)
	
#define bitset_free(jmeno_pole)\
	if (jmeno_pole != NULL){\
		free(jmeno_pole);\
	}
#endif