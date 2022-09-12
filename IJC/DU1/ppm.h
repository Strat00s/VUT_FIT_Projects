// ppm.h
// Řešení IJC-DU1 (19.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Header file pro definování "ppm" funkcí a struktury

#include <stdio.h>
#include <stdlib.h>

struct ppm {
    unsigned xsize;
    unsigned ysize;
    char data[];    // RGB bajty, celkem 3*xsize*ysize
};

struct ppm * ppm_read(const char * filename);

void ppm_free(struct ppm *p);