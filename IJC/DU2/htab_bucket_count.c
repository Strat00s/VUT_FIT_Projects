// htab_bucket_count.c
// Řešení IJC-DU2 (21.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Funkce vracející velikost hash tablu

#include "htab.h"
#include "structs.h"

size_t htab_bucket_count(const htab_t * t){
    return t->arr_size;
}