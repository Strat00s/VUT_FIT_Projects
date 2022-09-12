// htab_iterator_get_key.c
// Řešení IJC-DU2 (21.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Funkce která vrátí key

#include "htab.h"
#include "structs.h"

htab_key_t htab_iterator_get_key(htab_iterator_t it){
    return it.ptr->key;
}