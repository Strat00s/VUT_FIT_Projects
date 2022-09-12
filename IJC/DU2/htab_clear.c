// htab_clear.c
// Řešení IJC-DU2 (21.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Funkce která vyčistí hash table

#include "htab.h"
#include "structs.h"

void htab_clear(htab_t * t){
    //variables to save what to remove
    struct htab_item *curr_item;
    struct htab_item *free_item;

    //go through the list
    for (size_t i = 0; i < t->arr_size; i++){
        
        curr_item = t->list[i]; //save current item

        while (curr_item != NULL){          //while not empty
            free_item = curr_item;          //save which to free
            curr_item = curr_item->inext;   //move to next item
            free((void *)free_item->key);   //free key from current item
            free(free_item);                //free current item
        }
        t->list[i] = NULL;  //NULL entry (as nothing is there anymore)
    }
    t->size  = 0;   //make size 0 (as nothing is present anymore)
}