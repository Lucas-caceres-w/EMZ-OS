#ifndef SCREEN_H
#define SCREEN_H

#define VIDEO_ADDRESS 0xB8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x07

#define RED_ON_WHITE 0xF4
#define WHITE_ON_BLUE 0x1F

/* Funciones públicas */
void kclear_screen(char *arg);
void print_prompt(char *name);
void kprint_at(char *message, int col, int row, char attribute_byte);
void screenlog(char *message);
void kprint_error(char *message); // Para mensajes de pánico en rojo
int get_cursor_offset();
void set_cursor_offset(int offset);
int get_screen_offset(int col, int row);

#endif