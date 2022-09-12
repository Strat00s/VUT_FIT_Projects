// ppm.c
// Řešení IJC-DU1 (19.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Kód pro načítání a částečné ověření obrázku pro "steg-decode.c"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "ppm.h"
#include "error.h"

struct ppm * ppm_read(const char * filename){
    FILE *file = fopen(filename, "r");
    if (file == NULL){
        warning_msg("Soubor %s se nepodařilo otevřít!", filename);
        return NULL;
    }

    int width, height, colors;
    width = height = 0;
    int num;

    // 2 is for our ppm file 
    if(fscanf(file, "P%d %d %d %d ",&num, &width, &height, &colors) != 4 || num !=6 || height <= 0 || width <= 0 || colors != 255){
        warning_msg("Soubor %s neni validní ppm soubor!", filename);
        fclose(file);
        return NULL;
    }

    unsigned long size = width*height*3*sizeof(char);

    struct ppm *img_data = malloc(size+sizeof(struct ppm)); //malloc enough for data (size) and enough for the struct itself (sizeof(struct ppm))

    if (img_data == NULL){
        warning_msg("Alokace se nezdařila!");
        fclose(file);
        return NULL;
    }

    img_data->xsize = width;
    img_data->ysize = height;

    if(fread(img_data->data, sizeof(char), size, file) != size){    //read data from file and save them to img_data->data
        warning_msg("Nepodařilo se nahrát data!");
        fclose(file);
        free(img_data);
        return NULL;
    }

    fclose(file);

    return img_data;
}

void ppm_free(struct ppm *p){
    if (p != NULL){
        free(p);
    }
}