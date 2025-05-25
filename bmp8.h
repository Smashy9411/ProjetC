//
// Created by mathp on 21/03/2025.
//

#ifndef BMP8_H
#define BMP8_H

#include <stdio.h>
#include <stdlib.h>

// Définition de la structure pour une image BMP 8 bits / c'est l'ensemble des informations qu'on va lire
typedef struct {
    unsigned char header[54];     // En-tête du fichier BMP
    unsigned char colorTable[1024]; // Table des couleurs
    unsigned char *data;          // Données des pixels
    unsigned int width;           // Largeur de l'image
    unsigned int height;          // Hauteur de l'image
    unsigned int colorDepth;      // Profondeur de couleur (8 bits)
    unsigned int dataSize;        // Taille des données de l'image
} t_bmp8;


t_bmp8 *bmp8_loadImage(const char *filename);
void bmp8_saveImage(const char *filename, t_bmp8 *img);
void bmp8_free(t_bmp8 *img);
void bmp8_printInfo(t_bmp8 *img);

void bmp8_negative(t_bmp8 *img);
void bmp8_brightness(t_bmp8 *img, int value);
void bmp8_threshold(t_bmp8 *img, int threshold);
void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize);

void bmp8_equalizeHistogram(t_bmp8 *img);

#endif // BMP8_H
