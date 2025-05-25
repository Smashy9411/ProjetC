#include "bmp24.h"
#include <string.h> // Pour memset
#include <math.h>

/**
 * Alloue la mémoire pour les pixels d'une image BMP 24 bits
 * @param width Largeur de l'image en pixels
 * @param height Hauteur de l'image en pixels
 * @return Pointeur vers un tableau 2D de pixels alloué, NULL en cas d'échec
 */
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

/**
 * Libère la mémoire occupée par les pixels d'une image
 * @param pixels Tableau 2D de pixels à libérer
 * @param height Hauteur de l'image (nombre de lignes à libérer)
 */
void bmp24_freeDataPixels(t_pixel **pixels, int height) {
    if (!pixels) return;
    for (int i = 0; i < height; i++) {
        free(pixels[i]);
    }
    free(pixels);
}

/**
 * Alloue une structure BMP24 vide
 * @param width Largeur de l'image
 * @param height Hauteur de l'image
 * @param colorDepth Profondeur de couleur (doit être 24 pour BMP24)
 * @return Pointeur vers l'image allouée, NULL en cas d'échec
 */
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

/**
 * Libère complètement une image BMP24
 * @param img Pointeur vers l'image à libérer
 */
void bmp24_free(t_bmp24 *img) {
    if (!img) return;
    bmp24_freeDataPixels(img->data, img->height);
    free(img);
}

/**
 * Charge une image BMP 24 bits depuis un fichier
 * @param filename Chemin vers le fichier BMP
 * @return Pointeur vers l'image chargée, NULL en cas d'erreur
 */
t_bmp24 *bmp24_loadImage(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Erreur : impossible d'ouvrir %s\n", filename);
        return NULL;
    }

    t_bmp24 *img = (t_bmp24 *)malloc(sizeof(t_bmp24));
    if (!img) return NULL;

    fread(&img->header, sizeof(t_bmp_header), 1, file);
    fread(&img->header_info, sizeof(t_bmp_info), 1, file);

    if (img->header_info.bits != 24) {
        printf("Erreur : l'image n'est pas en 24 bits.\n");
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

/**
 * Sauvegarde une image BMP 24 bits dans un fichier
 * @param img Image à sauvegarder
 * @param filename Chemin du fichier de destination
 */
void bmp24_saveImage(t_bmp24 *img, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Erreur : impossible d'écrire dans %s\n", filename);
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

/**
 * Applique un filtre négatif à l'image (inversion des couleurs)
 * @param img Image à modifier
 */
void bmp24_negative(t_bmp24 *img) {
    for (int i = 0; i < img->height; i++) {
        for (int j = 0; j < img->width; j++) {
            img->data[i][j].red = 255 - img->data[i][j].red;
            img->data[i][j].green = 255 - img->data[i][j].green;
            img->data[i][j].blue = 255 - img->data[i][j].blue;
        }
    }
}

/**
 * Convertit l'image en niveaux de gris (moyenne des canaux RGB)
 * @param img Image à convertir
 */
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

/**
 * Ajuste la luminosité de l'image
 * @param img Image à modifier
 * @param value Valeur d'ajustement (peut être négative)
 */
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

/**
 * Effectue une convolution sur un pixel de l'image
 * @param img Image source
 * @param x Position x du pixel
 * @param y Position y du pixel
 * @param kernel Matrice de convolution
 * @param kernelSize Taille du noyau (doit être impair)
 * @return Pixel résultant de la convolution
 */
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

/**
 * Applique un filtre générique à l'image à partir d'un noyau de convolution
 * @param img Image à modifier
 * @param kernel Noyau de convolution
 * @param kernelSize Taille du noyau
 */
void bmp24_applyFilter(t_bmp24 *img, float **kernel, int kernelSize) {
    t_pixel **newData = bmp24_allocateDataPixels(img->width, img->height);
    if (!newData) return;

    int offset = kernelSize / 2;

    for (int y = offset; y < img->height - offset; y++) {
        for (int x = offset; x < img->width - offset; x++) {
            newData[y][x] = bmp24_convolution(img, x, y, kernel, kernelSize);
        }
    }

    // Remplacer l'ancienne data
    bmp24_freeDataPixels(img->data, img->height);
    img->data = newData;
}

/**
 * Applique un flou gaussien 3x3 à l'image
 * @param img Image à modifier
 */
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

/**
 * Applique un effet de relief (emboss) à l'image
 * @param img Image à modifier
 */
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

/**
 * Applique un filtre de netteté à l'image
 * @param img Image à modifier
 */
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

/**
 * Égalise l'histogramme de l'image pour améliorer le contraste
 * @param img Image à modifier
 */
void bmp24_equalizeHistogram(t_bmp24 *img) {
    if (!img || !img->data) return;

    int width = img->width;
    int height = img->height;
    int histogram[256] = {0};

    float *Y = (float *)malloc(width * height * sizeof(float));

    // Convert RGB to Y (luminance)
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            t_pixel p = img->data[y][x];
            int index = y * width + x;
            Y[index] = 0.299f * p.red + 0.587f * p.green + 0.114f * p.blue;
            histogram[(int)Y[index]]++;
        }
    }

    // Calcul du CDF (Cumulative Distribution Function)
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

    // Normalisation
    float scale = 255.0f / (width * height - cdf_min);
    unsigned char equalized[256];
    for (int i = 0; i < 256; i++) {
        equalized[i] = (unsigned char)(fmaxf(0, roundf((cdf[i] - cdf_min) * scale)));
    }

    // Application de l'égalisation
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = y * width + x;
            float newY = equalized[(int)Y[index]];
            t_pixel *p = &img->data[y][x];

            // Conversion YUV vers RGB
            float U = -0.14713f * p->red - 0.28886f * p->green + 0.436f * p->blue;
            float V = 0.615f * p->red - 0.51499f * p->green - 0.10001f * p->blue;

            int r = roundf(newY + 1.13983f * V);
            int g = roundf(newY - 0.39465f * U - 0.58060f * V);
            int b = roundf(newY + 2.03211f * U);

            p->red = (r > 255) ? 255 : (r < 0 ? 0 : r);
            p->green = (g > 255) ? 255 : (g < 0 ? 0 : g);
            p->blue = (b > 255) ? 255 : (b < 0 ? 0 : b);
        }
    }

    free(Y);
}