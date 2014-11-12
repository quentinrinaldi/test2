// outils.h

#ifndef HEADER_TOOLS
#define HEADER_TOOLS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _buffer {
    char *donnees;
    int taille;
    int ind;
} Buffer;

int char_to_int (char c);
char int_to_char (int i);

unsigned char *int_to_bytes(int i);
int bytes_to_int(unsigned char* octets);
void afficher_buffer (Buffer *buffer);
#endif