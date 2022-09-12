// htab_free.c
// Řešení IJC-DU2 (21.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Funkce která freene celý hash table

#include "htab.h"
#include "structs.h"

void htab_free(htab_t * t){
    htab_clear(t);
    free(t);
}