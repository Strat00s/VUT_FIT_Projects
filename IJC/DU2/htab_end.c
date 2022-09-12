// htab_end.c
// Řešení IJC-DU2 (21.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Funkce vracející neexistujici posledni+1 prvek

#include "htab.h"
#include "structs.h"

htab_iterator_t htab_end(const htab_t * t){
    return it_init(NULL, t, t->arr_size);
}