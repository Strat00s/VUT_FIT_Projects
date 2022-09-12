// htab_init.c
// Řešení IJC-DU2 (21.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Funkce pro inicializaci hash tablu

#include "htab.h"
#include "structs.h"

htab_t *htab_init(size_t n){
    htab_t *hash_table = malloc(sizeof(htab_t)+n*sizeof(struct htab_item *));   //htab_t itself + number of items
    //if allocation fails, return NULL
    if (hash_table == 0){
        return NULL;
    }
    hash_table->size = 0;
    hash_table->arr_size = n;
    
    //NULL all items
    for (size_t i = 0; i < n; i++){
        hash_table->list[i] = NULL;
    }
    return hash_table;  //if allocation fails, pointer will be null, so we will return NULL as well (I guess)
}