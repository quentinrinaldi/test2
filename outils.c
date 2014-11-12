#include "outils.h"

int char_to_int (char c) {
    return c - '0';
}

char int_to_char (int i) {
    return (char)(((int)'0')+i);
}

void swap_bytes(unsigned char *t)
{
    unsigned char tmp;
    
    tmp = t[0];
    t[0] = t[3];
    t[3] = tmp;
    
    tmp = t[1];
    t[1] = t[2];
    t[2] = tmp;
}

unsigned char *int_to_bytes (int n) {
    unsigned char octets[4];
    
    octets[0] = n >> 0;
    octets[1] = n >> 8;
    octets[2] = n >> 16;
    octets[3] = n >> 24;
    unsigned char *res = (unsigned char *) malloc (sizeof (unsigned char) * 4);
    // char res[4];
   
    
#ifdef LITTLE_ENDIAN
    swap_bytes(octets);
#endif
   
    memcpy(res, octets, sizeof(unsigned char) * 4);
    return res;
}

int bytes_to_int (unsigned char *octets) {
#ifdef LITTLE_ENDIAN
    swap_bytes(octets);
#endif
    int n = 0;
    
    for (int i=0, j=0; i<4; i++, j=j+8) {
        n += ((int) octets[i]) << j;
        
    }
    return n;
}

Buffer *creer_buffer (int taille_reelle, int taille_max) {
    Buffer *buffer = (Buffer *) malloc(sizeof(Buffer));
    buffer->taille=taille_reelle;
    buffer->ind = 0;
    buffer->donnees = (char *) malloc(sizeof(char) * taille_max);
    
    return buffer;
}

void afficher_buffer (Buffer *buffer) {
    printf("\n\n-------- Affichage buffer --------\n");
    for (int i=0; i<buffer->taille; i++) {
        printf("%d ",buffer->donnees[i]);
    }
    printf("\n\n");
}