// htab_lookup_add.c
// Řešení IJC-DU2 (21.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Funkce která přidá prvek

#include "htab.h"
#include "structs.h"
//#include <stdio.h>

htab_iterator_t htab_lookup_add(htab_t *t, htab_key_t key){

    //try to find the value first
    htab_iterator_t tmp = htab_find(t, key);
    if(tmp.ptr != NULL){
        htab_iterator_set_value(tmp, tmp.ptr->data+1);
        return tmp;
    }

    //create hash value and find the index
    unsigned int value = htab_hash_fun(key);    //uint
    size_t idx = value % t->arr_size;

    struct htab_item *item = malloc(sizeof(struct htab_item));
    //if malloc fails, return NULL;
    if (item == NULL){
        return htab_end(t);
    }

    item->data = 1;
    item->inext = NULL;
    
    char *key_holder = malloc(strlen(key) + 1);
    if (key_holder == NULL) {
        return htab_end(t);
    }

    strcpy(key_holder, key);
    item->key = key_holder;

    //struct htab_item *parrent = malloc(sizeof(struct htab_item));
    if (t->list[idx] != NULL){
        tmp.ptr = t->list[idx];
        while(tmp.ptr->inext != NULL){
            tmp.ptr = tmp.ptr->inext;
        }
        tmp.ptr->inext = item;
        t->size++;
        tmp = it_init(item, t, idx);
        return tmp;
    }
    t->list[idx] = item;
    t->size++;
    tmp = it_init(item, t, idx);
    return tmp;
}