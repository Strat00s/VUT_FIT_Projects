// htab_find.c
// Řešení IJC-DU2 (21.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Funkce která najde prvek

#include "htab.h"
#include "structs.h"

htab_iterator_t htab_find(htab_t * t, htab_key_t key){

    //variable to save current item
    struct htab_item *curr_item;

    //search by the hash (so that we don't have to go trhough the entire hash table)
    unsigned int value = htab_hash_fun(key);    //uint
    size_t idx = value % t->arr_size;

    //iterate untill at the end
    curr_item = t->list[idx];

    //go to the end
    while (curr_item != NULL){
        if (strcmp(key, curr_item->key) == 0){  //if we find a mach
            return it_init(curr_item, t, idx);
        }
        //move to next item
        curr_item = curr_item->inext;
    }
    //once at the end, return end;
    return htab_end(t);
}