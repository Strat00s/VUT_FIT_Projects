// primes.c
// Řešení IJC-DU1 (19.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Kód na hledání a následné vytištění posledních 10ti prvočísel z 500000000 (500M)

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>

#include "error.h"
#include "eratosthenes.h"

int main(){
    clock_t start = clock();    //start clock
    bitset_create(q, 500000000);   //create bit field
    eratosthenes(q); //run eratothenes

    int wanted = 0; //last 10 numbers counter

    //count from last number to 10th last prime number
    for (unsigned long i = 500000000-1; i > 1; i--){
        if (!(bitset_getbit(q, i))){
            wanted++;
            if (wanted == 10){
                
                //go back up and print the primes
                for (; i < 500000000; i++){
                    if (!(bitset_getbit(q, i))){
                        printf("%ld\n", i);
                    }
                }
                i = 1;  //1 because index is unsigned -> 1-1 = 0
            }
        }
    }
    fprintf(stderr, "Time=%.3g\n", (double)(clock()-start)/CLOCKS_PER_SEC); //print elapsed time
    return 0;
}
