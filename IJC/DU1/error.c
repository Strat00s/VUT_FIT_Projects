// error.c
// Řešení IJC-DU1 (19.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Kód na tištění chybových hlášení pomocí variadických funkcí

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "error.h"

#define VARIADIC_PRINT fprintf(stderr, "CHYBA: "); va_list args; va_start(args, fmt); vfprintf(stderr, fmt, args); va_end(args)

void warning_msg(const char *fmt, ...){
    VARIADIC_PRINT;
}

void error_exit(const char *fmt, ...){
    VARIADIC_PRINT;
    exit(1);
}