// wordcount.c
// Řešení IJC-DU2 (21.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Program který vytiskne jednotlivá slova a jejich četnost (ze stdin)

#include "htab.h"
#include "structs.h"
#include "io.h"

#include <stdio.h>

#define ARR_SIZE 40000//14716  //number of unique words in quran
#define MAX_ARGS 2      //maximu number of arguments are 2
#define MAX_KEY_LENGHT 128

int main(){

    htab_t *hash_table = htab_init(ARR_SIZE);   //initialize table
    htab_iterator_t iterator;                   //create iterator

    char *tmp = malloc(sizeof(char)*MAX_KEY_LENGHT);
    int lenght = 0;

    //while not at the end of file
    while(lenght != EOF){
        lenght = get_word(tmp, MAX_KEY_LENGHT, stdin);      //read words (128 max lenght)
        if(tmp[0] == '\0') break;
        iterator = htab_lookup_add(hash_table, tmp);    //add them to hash table
    }

    free(tmp);  //free tmp (where key is saved) as we don't need it anymore

    /***(printing)***/
    iterator = htab_begin(hash_table);  //go to start

    //print item while ptr (item) is not NULL
    while(true){
        printf("%s\t%d\n", iterator.ptr->key, iterator.ptr->data);
        iterator = htab_iterator_next(iterator);    //go to next item
        if (iterator.ptr == NULL) break;
    }

    htab_free(hash_table);  //free hash_table
    return 0;
}
