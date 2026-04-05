#include "drivers/screen.h"
#include "libs/util.h"
#include "libs/stdarg.h"

// Esta es la función "caballito de batalla" que procesa el formato
void klog(char *format, ...) {
  va_list args;
  va_start(args, format);

  for (char *ptr = format; *ptr != '\0'; ptr++) {
    // Si no es un especificador de formato, imprimimos el char normal
    if (*ptr != '%') {
      // Usamos kprint con un pequeño buffer para un solo char
      // o puedes crear kput_char(char c) en screen.c
      char temp[2] = {*ptr, '\0'};
      screenlog(temp);
      continue;
    }

    ptr++; // Saltamos el '%'

    switch (*ptr) {
    case 'c': {
      char c = (char)va_arg(args, int);
      char temp[2] = {c, '\0'};
      screenlog(temp);
      break;
    }
    case 's': {
      char *s = va_arg(args, char *);
      screenlog(s);
      break;
    }
    case 'd': {
      int d = va_arg(args, int);
      char buffer[32];
      itoa(d, buffer, 10);
      screenlog(buffer);
      break;
    }
    case 'x': {
      int x = va_arg(args, int);
      char buffer[32];
      itoa(x, buffer, 16);
      screenlog("0x");
      screenlog(buffer);
      break;
    }
    case '%': {
      screenlog("%");
      break;
    }
    }
  }

  va_end(args);
}