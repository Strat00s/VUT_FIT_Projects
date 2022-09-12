// error.h
// Řešení IJC-DU1 (19.3.2020)
// Autor: Lukáš Baštýř, FIT
// Přeloženo: gcc 9.2.1
// Header file pro definování "error" funkcí

#include <stdio.h>
#include <stdlib.h>

void warning_msg(const char *fmt, ...);
void error_exit(const char *fmt, ...);