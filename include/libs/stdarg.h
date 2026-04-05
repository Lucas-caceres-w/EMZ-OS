#ifndef STDARG_H
#define STDARG_H

typedef char* va_list;

// Determina el tamaño de un tipo alineado a la palabra (4 bytes en x86)
#define __va_argsiz(t) (((sizeof(t) + sizeof(int) - 1) & ~(sizeof(int) - 1)))

// Inicializa ap para que apunte al primer argumento variable
#define va_start(ap, last) ((ap) = (va_list)&(last) + __va_argsiz(last))

// Obtiene el siguiente argumento del tipo t y avanza el puntero ap
#define va_arg(ap, t) (*(t*)((ap) += __va_argsiz(t), (ap) - __va_argsiz(t)))

// Finaliza el uso de la lista (en x86 no suele hacer nada, pero es buena práctica)
#define va_end(ap) ((ap) = (va_list)0)

#endif