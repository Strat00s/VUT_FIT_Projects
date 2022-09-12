// eratosthenes.c
// Řešení IJC-DU1 (19.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Kod pro hledaní prvočísel pomocí eratosthenesova síta

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "eratosthenes.h"

void eratosthenes(bitset_t jmeno_pole){

    //go up to square root of size
    double end = sqrt(bitset_size(jmeno_pole));

    //start from number 2 (as 0 and 1 are not prime numbers)
    for (unsigned long i = 2; i < end; i++){
        //when on zeros
        if (!(bitset_getbit(jmeno_pole, i))){
            //set all multiples of i as 1 (non-prime)
            for (unsigned long x = 2; x*i < bitset_size(jmeno_pole); x++){
                bitset_setbit(jmeno_pole, x*i, 1); // set non-prime numbers as 1
            }
        }
    }
}