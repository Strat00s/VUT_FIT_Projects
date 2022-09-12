// htab_iterator_set_value.c
// Řešení IJC-DU2 (21.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Funkce která nastaví data na požadovanou hodnotu

#include "htab.h"
#include "structs.h"

htab_value_t htab_iterator_set_value(htab_iterator_t it, htab_value_t val){
    it.ptr->data = val;
    return val;
}