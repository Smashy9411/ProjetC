#include "bmp8.h"
#include <math.h> // pour round() utilisé dans bmp8_applyFilter si besoin
#include <stdlib.h>
#include <dirent.h>


// Charger une image BMP 8 bits
t_bmp8 *bmp8_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Erreur : Impossible d'ouvrir le fichier %s\n", filename);

        return NULL;
    }

    t_bmp8 *img = (t_bmp8 *)malloc(sizeof(t_bmp8));
    if (!img) {
        printf("Erreur : Allocation mémoire échouée\n");
        fclose(file);
        return NULL;
    }

    fread(img->header, sizeof(unsigned char), 54, file);

    img->width = *(unsigned int *)&img->header[18];
    img->height = *(unsigned int *)&img->header[22];
    img->colorDepth = *(unsigned short *)&img->header[28];

    if (img->colorDepth != 8) {
        printf("Erreur : Ce programme prend uniquement les images BMP 8 bits.\n");
        free(img);
        fclose(file);
        return NULL;
    }

    fread(img->colorTable, sizeof(unsigned char), 1024, file);

    // Lire l'offset vers les données d'image
    unsigned int dataOffset = *(unsigned int *)&img->header[10];
    fseek(file, dataOffset, SEEK_SET);

    img->dataSize = img->width * img->height;
    img->data = (unsigned char *)malloc(img->dataSize);
    if (!img->data) {
        printf("Erreur : Allocation mémoire pour les données échouée\n");
        free(img);
        fclose(file);
        return NULL;
    }

    fread(img->data, sizeof(unsigned char), img->dataSize, file);

    fclose(file);
    return img;
}


// Sauvegarder une image BMP 8 bits
void bmp8_saveImage(const char *filename, t_bmp8 *img) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Erreur : Impossible de créer le fichier %s\n", filename);
        return;
    }

    fwrite(img->header, sizeof(unsigned char), 54, file);
    fwrite(img->colorTable, sizeof(unsigned char), 1024, file);
    fwrite(img->data, sizeof(unsigned char), img->dataSize, file);

    fclose(file);
}

// Libérer la mémoire d'une image BMP 8 bits
void bmp8_free(t_bmp8 *img) {
    if (img) {
        if (img->data) {
            free(img->data);
        }
        free(img);
    }
}

// Afficher les infos d'une image BMP 8 bits
void bmp8_printInfo(t_bmp8 *img) {
    if (img) {
        printf("\n--- Informations de l'image ---\n");
        printf("Dimensions : %dx%d\n", img->width, img->height);
        printf("Profondeur de couleur : %d bits\n", img->colorDepth);
        printf("Taille des données : %d octets\n", img->dataSize);
    } else {
        printf("Aucune image chargée.\n");
    }
}

// Inverser les couleurs (négatif)
void bmp8_negative(t_bmp8 *img) {
    if (img) {
        for (unsigned int i = 0; i < img->dataSize; i++) {
            img->data[i] = 255 - img->data[i];
        }
    }
}

// Modifier la luminosité
void bmp8_brightness(t_bmp8 *img, int value) {
    if (img) {
        for (unsigned int i = 0; i < img->dataSize; i++) {
            int pixel = img->data[i] + value;
            if (pixel > 255) pixel = 255;
            if (pixel < 0) pixel = 0;
            img->data[i] = (unsigned char)pixel;
        }
    }
}

// Appliquer un seuillage (binarisation)
void bmp8_threshold(t_bmp8 *img, int threshold) {
    if (img) {
        for (unsigned int i = 0; i < img->dataSize; i++) {
            img->data[i] = (img->data[i] >= threshold) ? 255 : 0;
        }
    }
}

// Appliquer un filtre par convolution
void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize) {
    if (!img || !kernel) return;

    int offset = kernelSize / 2;
    unsigned char *newData = (unsigned char *)malloc(img->dataSize);
    if (!newData) {
        printf("Erreur : Allocation mémoire échouée pour le filtrage.\n");
        return;
    }

    for (unsigned int y = offset; y < img->height - offset; y++) {
        for (unsigned int x = offset; x < img->width - offset; x++) {
            float sum = 0.0f;

            for (int ky = -offset; ky <= offset; ky++) {
                for (int kx = -offset; kx <= offset; kx++) {
                    int px = x + kx;
                    int py = y + ky;
                    sum += img->data[py * img->width + px] * kernel[ky + offset][kx + offset];
                }
            }

            int pixel = (int)roundf(sum);
            if (pixel > 255) pixel = 255;
            if (pixel < 0) pixel = 0;
            newData[y * img->width + x] = (unsigned char)pixel;
        }
    }

    // Copier newData dans img->data
    for (unsigned int y = offset; y < img->height - offset; y++) {
        for (unsigned int x = offset; x < img->width - offset; x++) {
            img->data[y * img->width + x] = newData[y * img->width + x];
        }
    }

    free(newData);
}
void bmp8_equalizeHistogram(t_bmp8 *img) {
    if (!img || !img->data) return;

    int histogram[256] = {0};
    for (unsigned int i = 0; i < img->dataSize; i++) {
        histogram[img->data[i]]++;
    }

    float cdf[256] = {0};
    cdf[0] = histogram[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + histogram[i];
    }

    float cdf_min = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf[i] != 0) {
            cdf_min = cdf[i];
            break;
        }
    }

    float scale = 255.0f / (img->width * img->height - cdf_min);

    unsigned char equalized[256];
    for (int i = 0; i < 256; i++) {
        equalized[i] = (unsigned char)(fmaxf(0, roundf((cdf[i] - cdf_min) * scale)));
    }

    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = equalized[img->data[i]];
    }
}

