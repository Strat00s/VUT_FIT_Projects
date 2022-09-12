/*  Project #2
    Diode calculations
    Lukas Bastyr, xbasty00
    08.11.2019

    What it is supposed to do:
    Calculate working voltage and current
    of a diode using bisection
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define I0 1e-12         // A
#define U_T 0.0258563    // V
#define U_MIN 0          // V
#define MAX_ITER 1000000

double diode(double u0, double r, double eps){

    // variables
    double amp_max, amp_piv, pivot;
    double x = U_MIN;
    double y = u0;
    int iter_cnt  = 0;
    
    do{
        // pivot, shockley + kirchov
        pivot = (x + y)/2;
        amp_max = I0*(exp(y/U_T)-1) - (u0 - y)/r;
        amp_piv = I0*(exp(pivot/U_T)-1) - (u0 - pivot)/r;

        // Don't run forever
        if (iter_cnt > MAX_ITER){
            return pivot;
        }
        // same znaménko -> move pivot
        if (amp_max*amp_piv > 0){
            y = pivot;
        }
        else {
            x = pivot;
        }
    iter_cnt++;

    // distance between endpoints
    } while(fabs(x-y) > eps);
    return pivot;
}

int main(int argc, char *argv[]){
    
    // check for correct number of arguments
    if (argc != 4){
        fprintf(stderr, "error: invalid arguments\n");
        return -1;
    }

    char *str;
    double u0 = strtod(argv[1], NULL);
    double r = strtod(argv[2], NULL);
    double eps = strtod(argv[3], NULL);

    // check if args are bigger than zeros (except u0)
    if (u0 >= 0 && r > 0 && r != INFINITY && eps > 0 ){
        
        // checking args for aplhas
        for (int i = 1; i < argc; i++){
            strtod(argv[i], &str);
            
            // if returned string isn't empty -> wrong argument
            if (str[0] != '\0'){
                fprintf(stderr, "error: invalid arguments\n");
                return -1;
            }
        }

        // write diode to up
        double up = diode(u0, r, eps);
        double ip = I0*(exp(up/U_T)-1);

        // print up and calculate ip
        printf("Up=%g V\nIp=%g A\n", up, ip);
        return 0;
    }
    fprintf(stderr, "error: invalid arguments\n");
    return -1;
}
