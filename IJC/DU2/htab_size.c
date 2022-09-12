// htab_size.c
// Řešení IJC-DU2 (21.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Funkce která vrátí počet prvků

#include "htab.h"
#include "structs.h"

size_t htab_size(const htab_t * t){
    return t->size;
}