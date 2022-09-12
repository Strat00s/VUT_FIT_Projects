// htab_erase.c
// Řešení IJC-DU2 (21.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Funkce která vymaže daný prvek

#include "htab.h"
#include "structs.h"

void htab_erase(htab_t * t, htab_iterator_t it){
    if (it.ptr == NULL){
        return;
    }
    struct htab_item *curr_item = malloc(sizeof(struct htab_item));
    //keeper->inext = curr_item;

    curr_item = t->list[it.idx];
    if (curr_item != it.ptr){
        while(curr_item->inext != it.ptr){
            //keeper = curr_item;
            curr_item = curr_item->inext;
        }
        curr_item->inext = it.ptr->inext;
        free((void *)it.ptr->key);
        free(it.ptr);
        return;
    }
    t->list[it.idx] = it.ptr->inext;
    free((void *)curr_item->key);
    free(curr_item);
}