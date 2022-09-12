// steg-decode.c
// Řešení IJC-DU1 (19.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Kód na hledání tajné zprávy v obrázku (ppm)

#include <stdio.h>
#include <stdlib.h>

#include "eratosthenes.h"
#include "ppm.h"
#include "error.h"

//setbit_getbit but not for unsigned long
int stegGetBit(char data[], int index){
    return ((data[index/SIZEOF(data)] & (1 << index%SIZEOF(data))) > 0);
}

//setbit_setbit but not for unsigned long
void stegSetBit(char *c, int index, int vyraz){
    if (vyraz){
		c[index/SIZEOF(c)] |= (1 << index%SIZEOF(c));
	}
	else{
		c[index/SIZEOF(c)] &= ~(1 << index%SIZEOF(c));
	}
}

void getMessage(struct ppm *img_data){
    
    unsigned long size = img_data->xsize*img_data->xsize*3; //get the size from img_data struct
    
    bitset_alloc(mask, size);   //alloc our bitset

    int index = 0;  //character bit index

    char c; //character variable
    
    eratosthenes(mask);

    for (unsigned long i = 2; i < size-1; i++){

        //if on prime number, but after 22nd number (we should start at number 23)
        if (!(bitset_getbit(mask, i)) && i > 22){

            //character bit = lsb data bit
            stegSetBit((&c), index, stegGetBit((&img_data->data[i]), 0));
            
            //got to next bit in c
            index++;
            
            //once all bits are set, print the character out
            if (index == CHAR_BIT){
                
                //end once at the end of the message
                if (c == '\0'){
                    break;
                }

                //print the characater and reset character bit index
                printf("%c", c);
				index = 0;
            }
        }
    }

    //if we are at the end and there is still no "end"
    if (c != '\0'){
        error_exit("Nepovedlo se najít ukončení zprávy!");
    }

    // using bitset_alloc, so we have to free our bitset
    bitset_free(mask);
}

int main(int argc, char *argv[]){

    //check for correct number of arguments
    if (argc != 2){
        error_exit("Špatný počet argumentů!");
    }
    
    struct ppm *img_data = ppm_read(argv[1]);

    //if loading failed
    if (img_data == NULL){
        ppm_free(img_data);
        error_exit("Chyba načítání!");
    }

    getMessage(img_data);

    ppm_free(img_data);

    return 0;
}