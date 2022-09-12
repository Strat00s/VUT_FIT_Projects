// htab_iterator_get_value.c
// Řešení IJC-DU2 (21.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Funkce která vrátí data

#include "htab.h"
#include "structs.h"
#include <stdio.h>

htab_value_t htab_iterator_get_value(htab_iterator_t it){
    return it.ptr->data;
}