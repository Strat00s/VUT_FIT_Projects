// io.c
// Řešení IJC-DU2 (21.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Program který vrátí jednotlivá slova ze souboru

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

#include "io.h"

int get_word(char *s, int max, FILE *f){

    static bool once = true;

    if (f == NULL){
        fprintf(stderr, "Soubor je NULL\n");
        return -1;
    }
    int c = fgetc(f);

    //skip all spaces before the word
    while(c != EOF && isspace(c)){
        c = fgetc(f);
    }

    int i = 0;
    //while not at the end
    while(c != EOF){
        // go until at the limit-1 (we have to be able to end the string)
        for (; i < max-1; i++){
            s[i] = c;           //save
            c = fgetc(f);       //read new character
            if (isspace(c) || c == EOF){    //if at the end of the word
                s[i+1] = '\0';  //end the string
                return i+1;     //return lenght
            }
        }
        //once the word is longer than the limit
        s[i] = '\0';          //end string
        c = fgetc(f);
        if (once == true){
            fprintf(stderr, "Nejake slovo je delsi nez implementacni limit!\n");
            once = false;
        }
        //read the word till the end
        while(!isspace(c)){
            c = fgetc(f);
            i++;
            if (c == EOF){
                return EOF;
            }
        }
        return i+1;
    }
    s[0] = '\0';
    return EOF;
}