#ifndef UTIL_H
#define UTIL_H

#include "libs/types.h"

// Declaramos la función (prototipo)
int strcmp(char s1[], char s2[]);
int strncmp(const char *s1, const char *s2, uint32_t n);
void strncpy(char *dest, char *src, int max);

void itoa(int n, char *str, int base);
int atoi(char *str);
void *memset(void *dest, int val, uint32_t len);

// Puedes añadir más utilidades aquí más adelante
void int_to_ascii(int n, char str[]);
void reverse(char s[]);
int strlen(char s[]);


#endif