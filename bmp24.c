#include <stdio.h>
#include <stdlib.h>
#include "bmp24.h"

t_bmp24* bmp24_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Erreur : Impossible d'ouvrir le fichier %s\n", filename);
        return NULL;
    }

    t_bmp24 *img = (t_bmp24*)malloc(sizeof(t_bmp24));
    if (!img) {
        printf("Erreur : Allocation mémoire échouée\n");
        fclose(file);
        return NULL;
    }

    fread(img->header, 1, 54, file);

    img->width = *(unsigned int*)&img->header[18];
    img->height = *(unsigned int*)&img->header[22];
    img->colorDepth = *(unsigned short*)&img->header[28];
    img->dataSize = *(unsigned int*)&img->header[34];

    if (img->colorDepth != 24) {
        printf("Erreur : L'image n'est pas en couleur 24 bits\n");
        free(img);
        fclose(file);
        return NULL;
    }

    img->data = (unsigned char*)malloc(img->dataSize);
    if (!img->data) {
        printf("Erreur : Allocation mémoire pour les données.\n");
        free(img);
        fclose(file);
        return NULL;
    }

    fread(img->data, 1, img->dataSize, file);
    fclose(file);
    return img;
}

void bmp24_saveImage(const char *filename, t_bmp24 *img) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Erreur : Impossible de créer le fichier %s\n", filename);
        return;
    }

    fwrite(img->header, 1, 54, file);
    fwrite(img->data, 1, img->dataSize, file);

    fclose(file);
}

void bmp24_free(t_bmp24 *img) {
    if (img) {
        if (img->data) free(img->data);
        free(img);
    }
}

void bmp24_printInfo(t_bmp24 *img) {
    printf("Image Info:\n");
    printf("Width: %u\n", img->width);
    printf("Height: %u\n", img->height);
    printf("Color Depth: %u\n", img->colorDepth);
    printf("Data Size: %u\n", img->dataSize);
}

void bmp24_negative(t_bmp24 *img) {
    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = 255 - img->data[i];
    }
}

void bmp24_grayscale(t_bmp24 *img) {
    for (unsigned int i = 0; i < img->dataSize; i += 3) {
        unsigned char blue = img->data[i];
        unsigned char green = img->data[i + 1];
        unsigned char red = img->data[i + 2];

        unsigned char gray = (red + green + blue) / 3;

        img->data[i] = gray;
        img->data[i + 1] = gray;
        img->data[i + 2] = gray;
    }
}

void bmp24_brightness(t_bmp24 *img, int value) {
    for (unsigned int i = 0; i < img->dataSize; i++) {
        int pixel = img->data[i] + value;
        if (pixel > 255) pixel = 255;
        if (pixel < 0) pixel = 0;
        img->data[i] = (unsigned char)pixel;
    }
}//
// Created by mathp on 27/04/2025.
//
