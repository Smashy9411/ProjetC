//
// Created by mathp on 21/03/2025.
//

#ifndef BMP8_H
#define BMP8_H

#include <stdio.h>
#include <stdlib.h>

// Définition de la structure pour une image BMP 8 bits (c'est avec ca que l'on va creer les filtres et tout)
typedef struct {
    unsigned char header[54];   // En-tête du fichier BMP (54 octets)
    unsigned char colorTable[1024];  // Table des couleurs (1024 octets pour 256 couleurs)
    unsigned char *data;  // Données des pixels
    unsigned int width;   // Largeur de l'image
    unsigned int height;  // Hauteur de l'image
    unsigned int colorDepth;  // Profondeur de couleur (8 bits pour niveaux de gris)
    unsigned int dataSize;  // Taille des données de l'image
} t_bmp8;


t_bmp8 *bmp8_loadImage(const char *filename);

#endif // BMP8_H
