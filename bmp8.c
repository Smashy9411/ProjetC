#include "bmp8.h"

t_bmp8 *bmp8_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Erreur : Impossible d'ouvrir le fichier %s\n", filename);
        return NULL;
    }

    // Allocation mémoire pour la structure
    t_bmp8 *img = (t_bmp8*)malloc(sizeof(t_bmp8));
    if (!img) {
        printf("Erreur : Allocation mémoire échouée\n");
        fclose(file);
        return NULL;
    }

    // Lire l'en-tête du fichier BMP (54 octets)
    fread(img->header, sizeof(unsigned char), 54, file);

    // Extraire les dimensions et la profondeur de couleur
    img->width = *(int*)&img->header[18]; // c'est  les adresses de ces informations voir doc si pas compris
    img->height = *(int*)&img->header[22];
    img->colorDepth = *(short*)&img->header[28];

    if (img->colorDepth != 8) {
        printf("Erreur : Ce programme ne prend en charge que les images BMP 8 bits.\n");
        free(img);
        fclose(file);
        return NULL;
    }

    // Lire la table des couleurs (1024 octets pour 256 couleurs)
    fread(img->colorTable, sizeof(unsigned char), 1024, file);

    // Calcul de la taille des données
    img->dataSize = img->width * img->height;
    img->data = (unsigned char*)malloc(img->dataSize);
    if (!img->data) {
        printf("Erreur : Impossible d'allouer la mémoire pour les données d'image.\n");
        free(img);
        fclose(file);
        return NULL;
    }

    // Lire les pixels
    fread(img->data, sizeof(unsigned char), img->dataSize, file);

    fclose(file);
    return img;
}
