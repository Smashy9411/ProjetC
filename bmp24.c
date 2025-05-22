#include "bmp24.h"
#include <string.h> // Pour memset
#include <math.h>

// Allouer la mémoire pour les pixels
t_pixel **bmp24_allocateDataPixels(int width, int height) {
    t_pixel **pixels = (t_pixel **)malloc(height * sizeof(t_pixel *));
    if (!pixels) return NULL;

    for (int i = 0; i < height; i++) {
        pixels[i] = (t_pixel *)malloc(width * sizeof(t_pixel));
        if (!pixels[i]) {
            for (int j = 0; j < i; j++) free(pixels[j]);
            free(pixels);
            return NULL;
        }
    }
    return pixels;
}

// Libérer les pixels
void bmp24_freeDataPixels(t_pixel **pixels, int height) {
    if (!pixels) return;
    for (int i = 0; i < height; i++) {
        free(pixels[i]);
    }
    free(pixels);
}

// Allouer une image vide
t_bmp24 *bmp24_allocate(int width, int height, int colorDepth) {
    t_bmp24 *img = (t_bmp24 *)malloc(sizeof(t_bmp24));
    if (!img) return NULL;

    img->width = width;
    img->height = height;
    img->colorDepth = colorDepth;

    img->data = bmp24_allocateDataPixels(width, height);
    if (!img->data) {
        free(img);
        return NULL;
    }

    return img;
}

// Libérer l'image
void bmp24_free(t_bmp24 *img) {
    if (!img) return;
    bmp24_freeDataPixels(img->data, img->height);
    free(img);
}

// Charger une image BMP 24 bits
t_bmp24 *bmp24_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Erreur : impossible d’ouvrir %s\n", filename);
        return NULL;
    }

    t_bmp24 *img = (t_bmp24 *)malloc(sizeof(t_bmp24));
    if (!img) return NULL;

    fread(&img->header, sizeof(t_bmp_header), 1, file);
    fread(&img->header_info, sizeof(t_bmp_info), 1, file);

    if (img->header_info.bits != 24) {
        printf("Erreur : l'image n’est pas en 24 bits.\n");
        fclose(file);
        free(img);
        return NULL;
    }

    img->width = img->header_info.width;
    img->height = img->header_info.height;
    img->colorDepth = img->header_info.bits;

    img->data = bmp24_allocateDataPixels(img->width, img->height);
    if (!img->data) {
        fclose(file);
        free(img);
        return NULL;
    }

    int padding = (4 - (img->width * 3) % 4) % 4;
    for (int i = img->height - 1; i >= 0; i--) {
        for (int j = 0; j < img->width; j++) {
            fread(&img->data[i][j].blue, 1, 1, file);
            fread(&img->data[i][j].green, 1, 1, file);
            fread(&img->data[i][j].red, 1, 1, file);
        }
        fseek(file, padding, SEEK_CUR);
    }

    fclose(file);
    return img;
}

// Sauvegarder une image BMP 24 bits
void bmp24_saveImage(t_bmp24 *img, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Erreur : impossible d’écrire dans %s\n", filename);
        return;
    }

    fwrite(&img->header, sizeof(t_bmp_header), 1, file);
    fwrite(&img->header_info, sizeof(t_bmp_info), 1, file);

    int padding = (4 - (img->width * 3) % 4) % 4;
    unsigned char pad[3] = {0, 0, 0};

    for (int i = img->height - 1; i >= 0; i--) {
        for (int j = 0; j < img->width; j++) {
            fwrite(&img->data[i][j].blue, 1, 1, file);
            fwrite(&img->data[i][j].green, 1, 1, file);
            fwrite(&img->data[i][j].red, 1, 1, file);
        }
        fwrite(pad, 1, padding, file);
    }

    fclose(file);
}

// Appliquer un filtre négatif
void bmp24_negative(t_bmp24 *img) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            img->data[i][j].red = 255 - img->data[i][j].red;
            img->data[i][j].green = 255 - img->data[i][j].green;
            img->data[i][j].blue = 255 - img->data[i][j].blue;
        }
    }
}

// Convertir en niveaux de gris
void bmp24_grayscale(t_bmp24 *img) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            uint8_t r = img->data[i][j].red;
            uint8_t g = img->data[i][j].green;
            uint8_t b = img->data[i][j].blue;
            uint8_t gray = (r + g + b) / 3;

            img->data[i][j].red = gray;
            img->data[i][j].green = gray;
            img->data[i][j].blue = gray;
        }
    }
}

// Modifier la luminosité
void bmp24_brightness(t_bmp24 *img, int value) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            int r = img->data[i][j].red + value;
            int g = img->data[i][j].green + value;
            int b = img->data[i][j].blue + value;

            img->data[i][j].red = (r > 255) ? 255 : (r < 0 ? 0 : r);
            img->data[i][j].green = (g > 255) ? 255 : (g < 0 ? 0 : g);
            img->data[i][j].blue = (b > 255) ? 255 : (b < 0 ? 0 : b);
        }
    }
}

#include <math.h> // pour round()

// Convolution sur un seul pixel
t_pixel bmp24_convolution(t_bmp24 *img, int x, int y, float **kernel, int kernelSize) {
    int offset = kernelSize / 2;
    float r = 0.0f, g = 0.0f, b = 0.0f;

    for (int ky = -offset; ky <= offset; ky++) {
        for (int kx = -offset; kx <= offset; kx++) {
            int px = x + kx;
            int py = y + ky;
            if (px >= 0 && px < img->width && py >= 0 && py < img->height) {
                t_pixel p = img->data[py][px];
                float coeff = kernel[ky + offset][kx + offset];
                r += p.red * coeff;
                g += p.green * coeff;
                b += p.blue * coeff;
            }
        }
    }

    t_pixel result;
    result.red = (uint8_t)fminf(fmaxf(roundf(r), 0), 255);
    result.green = (uint8_t)fminf(fmaxf(roundf(g), 0), 255);
    result.blue = (uint8_t)fminf(fmaxf(roundf(b), 0), 255);

    return result;
}

// Application générique d’un filtre avec un kernel
void bmp24_applyFilter(t_bmp24 *img, float **kernel, int kernelSize) {
    t_pixel **newData = bmp24_allocateDataPixels(img->width, img->height);
    if (!newData) return;

    int offset = kernelSize / 2;

    for (int y = offset; y < img->height - offset; y++) {
        for (int x = offset; x < img->width - offset; x++) {
            newData[y][x] = bmp24_convolution(img, x, y, kernel, kernelSize);
        }
    }

    // Remplacer l’ancienne data
    bmp24_freeDataPixels(img->data, img->height);
    img->data = newData;
}

// === Filtres spécifiques ===

void bmp24_boxBlur(t_bmp24 *img) {
    int k[3][3] = {{1,1,1},{1,1,1},{1,1,1}};
    float **kernel = (float **)malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel[i] = (float *)malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) kernel[i][j] = k[i][j] / 9.0f;
    }
    bmp24_applyFilter(img, kernel, 3);
    for (int i = 0; i < 3; i++) free(kernel[i]);
    free(kernel);
}

void bmp24_gaussianBlur(t_bmp24 *img) {
    int k[3][3] = {{1,2,1},{2,4,2},{1,2,1}};
    float **kernel = (float **)malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel[i] = (float *)malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) kernel[i][j] = k[i][j] / 16.0f;
    }
    bmp24_applyFilter(img, kernel, 3);
    for (int i = 0; i < 3; i++) free(kernel[i]);
    free(kernel);
}

void bmp24_outline(t_bmp24 *img) {
    int k[3][3] = {{-1,-1,-1},{-1,8,-1},{-1,-1,-1}};
    float **kernel = (float **)malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel[i] = (float *)malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) kernel[i][j] = (float)k[i][j];
    }
    bmp24_applyFilter(img, kernel, 3);
    for (int i = 0; i < 3; i++) free(kernel[i]);
    free(kernel);
}

void bmp24_emboss(t_bmp24 *img) {
    int k[3][3] = {{-2,-1,0},{-1,1,1},{0,1,2}};
    float **kernel = (float **)malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel[i] = (float *)malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) kernel[i][j] = (float)k[i][j];
    }
    bmp24_applyFilter(img, kernel, 3);
    for (int i = 0; i < 3; i++) free(kernel[i]);
    free(kernel);
}

void bmp24_sharpen(t_bmp24 *img) {
    int k[3][3] = {{0,-1,0},{-1,5,-1},{0,-1,0}};
    float **kernel = (float **)malloc(3 * sizeof(float *));
    for (int i = 0; i < 3; i++) {
        kernel[i] = (float *)malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) kernel[i][j] = (float)k[i][j];
    }
    bmp24_applyFilter(img, kernel, 3);
    for (int i = 0; i < 3; i++) free(kernel[i]);
    free(kernel);
}

