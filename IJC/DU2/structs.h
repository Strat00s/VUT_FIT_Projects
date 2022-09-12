// structs.h
// Řešení IJC-DU2 (21.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Header s definicema struktur pro hash table a itemy

#include <stdlib.h>

#include "htab.h"

struct htab{
    size_t size;                //size
    size_t arr_size;            //size of next item
    struct htab_item *list[];    //array of items in hash table (array of pointers)
};

struct htab_item{
    htab_key_t key;                  //"data"
    htab_value_t data;          //counter
    struct htab_item *inext;    //pointer to next item
};

htab_iterator_t it_init(struct htab_item *ptr, const htab_t *t, size_t idx);