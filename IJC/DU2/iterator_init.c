// iterator_init.c
// Řešení IJC-DU2 (21.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Fuknce na inicializaci iteratoru

#include "structs.h"

htab_iterator_t it_init(struct htab_item *ptr, const htab_t *t, size_t idx){
    //htab_iterator_t pepa = {ptr, t, idx};
    htab_iterator_t pepa;
    pepa.ptr = ptr;
    pepa.t = t;
    pepa.idx = idx;
    return pepa;
}