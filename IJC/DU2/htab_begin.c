// htab_begin.c
// Řešení IJC-DU2 (21.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Funkce která vrátí první prvek v hasg tablu

#include "htab.h"
#include "structs.h"

htab_iterator_t htab_begin(const htab_t * t){
    for (size_t i = 0; i < t->arr_size; i++){
        if (t->list[i] != NULL){
            return it_init(t->list[i], t, i);
        }
    }
    return it_init(NULL, t, 0);
}