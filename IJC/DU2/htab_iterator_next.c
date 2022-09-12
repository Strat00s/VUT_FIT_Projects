// htab_iterator_next.c
// Řešení IJC-DU2 (21.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Funkce která vrátí následující prvek

#include "htab.h"
#include "structs.h"

htab_iterator_t htab_iterator_next(htab_iterator_t it){
    //if at the end, go to the next list. If at the total end, return htab_end
    if (it.ptr->inext == NULL){
        //+1 is important here
        for (size_t i = it.idx+1; i < it.t->arr_size; i++){
            if (it.t->list[i] != NULL){
                it.ptr = it.t->list[i];
                it.idx = i;
                return it;
            }
        }
        return htab_end(it.t);
    }
    //if none of the above, simply return next item.
    it.ptr = it.ptr->inext;
    return it;
}