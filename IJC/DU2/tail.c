// tail.c
// Řešení IJC-DU2 (21.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Program na čtení x posledních řádků ze souboru/stdin

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>

#define LINE_LENGHT 1023
#define DEFAULT_LINES 10

int checkForDigit(char argv[], unsigned int start){
    for ( ; start < strlen(argv); start++){
        if (!isdigit(argv[start])) return 0;
    }
    return 1;
}

int getArgs(int argc, char *argv[], int *file_arg_pos, bool *from_start){

    if (argc == 3 || argc == 4){

        //check that 2nd argument is actually -n
        if (!strcmp(argv[1], "-n")){
            *file_arg_pos = 3;   //file is going to be 4th (3+1) argument

            //check that the 3rd argument has digits only
            if (checkForDigit(argv[2], 0)){
                return atoi(argv[2]);
            }

            //else check that + or - is on first possition and rest are numbers
            else if (argv[2][0] == '+' && checkForDigit(argv[2], 1)){
                *from_start = true;  //gonna go from start
                return atoi(argv[2]);
            }

            else if (argv[2][0] == '-' && checkForDigit(argv[2], 1)){
                return -(atoi(argv[2]));
            }

            //TODO duplicated code
            //otherwise we have wrong arguments
            else{
                return -1;
            }
        }
        else {
            return -1;
        }
    }
    else if (argc== 1 || argc == 2){
        return DEFAULT_LINES;
    }
    return -1;
}

void printLines(FILE * tail_file){
    int char_cnt = 0;
    static bool long_err = true;
    int c;
    for (c = fgetc(tail_file); c != EOF; c = fgetc(tail_file)){
        char_cnt++;
        if (char_cnt > LINE_LENGHT){
            if (long_err){
                fprintf(stderr, "Radek ma prilis mnoho characteru.\n");
                long_err = false;
            }
            if (c == '\n') printf("\n");
        }
        else{
            printf("%c", c);
        }
        if (c == '\n') char_cnt = 0;
    }
}

void goFromEnd(FILE * tail_file, int max_lines){
    int c = 0;
    int eof_cnt = 0;
    int offset = -1;
    bool once = true;
    while(eof_cnt < max_lines){
        fseek(tail_file, offset, SEEK_END);
        c = fgetc(tail_file);
        //printf("%c",c);
        //if last char (enf of file) ends with \n, skip it
        if (once && c == '\n'){
            c = 0;
            once = false;
        }
        offset--;
        if (c == '\n'){
            eof_cnt++;
        }
    }
}

void goFromStart(FILE * tail_file, int max_lines){
    int c = 0;
    int eof_cnt = 0;
    while(eof_cnt < max_lines-1){
        c = fgetc(tail_file);
        if (c == '\n'){
            eof_cnt++;
        }
    }
}

int main (int argc, char *argv[]){

    int file_arg_pos = 1;
    int max_lines;
    bool from_start = false;

    max_lines = getArgs(argc, argv, &file_arg_pos, &from_start);
    if (max_lines == -1){
        fprintf(stderr, "Spatny pocet argumentu.\n");
        return -1;
    }
    
    //printf("max_lines: %d\n", max_lines);
    FILE *tail_file;
    if (argc == 2 || argc == 4){
        tail_file = fopen(argv[file_arg_pos], "r");
        if (tail_file == NULL){
            fprintf(stderr, "Soubor se nepodarilo otevrit.\n");
            return -1;
        }
    }
    else {
        tail_file = stdin;
        int c = fgetc(stdin);
        while(true){
            c = fgetc(stdin);
            if (c == EOF){
                break;
            }
        }
    }
    if (!from_start && max_lines == 0) goto end;
    
    if (from_start ){
        goFromStart(tail_file, max_lines);
    }
    else{
        goFromEnd(tail_file, max_lines);
    }

    printLines(tail_file);

    end:
    if ((argc == 2 || argc == 4) && tail_file != NULL){
        free(tail_file);
    }

   return 0;
}
