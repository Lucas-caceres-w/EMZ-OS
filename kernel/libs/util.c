#include "libs/types.h"

int strcmp(char s1[], char s2[]) {
  int i;
  for (i = 0; s1[i] == s2[i]; i++) {
    if (s1[i] == '\0')
      return 0;
  }
  return s1[i] - s2[i];
}

void strncpy(char *dest, char *src, int max) {
  int i = 0;
  while (i < max - 1 && src[i] != '\0' && src[i] != '\n') {
    dest[i] = src[i];
    i++;
  }
  dest[i] = '\0'; // Aseguramos el cierre del string
}

int strncmp(const char *s1, const char *s2, uint32_t n) {
  while (n > 0 && *s1 && (*s1 == *s2)) {
    s1++;
    s2++;
    n--;
  }
  if (n == 0)
    return 0;
  return *(unsigned char *)s1 - *(unsigned char *)s2;
}

uint32_t strlen(const char *str) {
  uint32_t len = 0;
  while (str[len] != '\0') {
    len++;
  }
  return len;
}

void itoa(int n, char *str, int base) {
  int i = 0;
  int is_negative = 0;

  if (n == 0) {
    str[i++] = '0';
    str[i] = '\0';
    return;
  }

  if (n < 0 && base == 10) {
    is_negative = 1;
    n = -n;
  }

  while (n != 0) {
    int rem = n % base;
    str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
    n = n / base;
  }

  if (is_negative)
    str[i++] = '-';
  str[i] = '\0';

  // Invertir la cadena (porque los dígitos salen al revés)
  int start = 0;
  int end = i - 1;
  while (start < end) {
    char temp = str[start];
    str[start] = str[end];
    str[end] = temp;
    start++;
    end--;
  }
}

int atoi(char *str) {
  int res = 0;
  for (int i = 0; str[i] != '\0' && str[i] != ' '; ++i) {
    if (str[i] >= '0' && str[i] <= '9') {
      res = res * 10 + str[i] - '0';
    }
  }
  return res;
}

void *memset(void *dest, int val, uint32_t len) {
  unsigned char *ptr = (unsigned char *)dest;
  while (len-- > 0) {
    *ptr++ = (unsigned char)val;
  }
  return dest;
}