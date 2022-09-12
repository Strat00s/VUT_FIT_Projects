// htab_hash_fun.c
// Řešení IJC-DU2 (21.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// (defaultni) Hashovaci funkce

#include <stdint.h>

#include "htab.h"

size_t htab_hash_fun(const char *str) {
    uint32_t h=0;     // musí mít 32 bitů
    const unsigned char *p;
    for(p=(const unsigned char*)str; *p!='\0'; p++)
        h = 65599*h + *p;
    return h;
}