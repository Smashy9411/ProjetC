//
// Created by mathp on 21/03/2025.
//
#include <stdio.h>
#include <stdlib.h>
#include "main.c"
int bmp8(void) {


  t_bmp8 * bmp8_loadImage(const char * filename){
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Erreur : Impossible d'ouvrir le fichier %s\n", filename);
        return NULL;
    }

}
