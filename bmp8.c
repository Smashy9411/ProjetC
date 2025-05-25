#include "bmp8.h"
#include <math.h>   // pour round()
#include <stdlib.h>
#include <dirent.h> // pour la gestion de répertoires si besoin

// === Fonction : bmp8_loadImage ===
// Paramètres :
//    - filename : chemin vers le fichier image BMP 8 bits à charger
// But :
//    - Charger une image BMP 8 bits en mémoire dans une structure t_bmp8
// Sortie :
//    - Retourne un pointeur vers la structure t_bmp8 si succès, sinon NULL
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

    // Lecture de l'en-tête BMP (54 octets)
    fread(img->header, sizeof(unsigned char), 54, file);

    // Récupération des métadonnées de l'image
    img->width = *(unsigned int *)&img->header[18];
    img->height = *(unsigned int *)&img->header[22];
    img->colorDepth = *(unsigned short *)&img->header[28];

    // Vérification que l'image est bien en 8 bits
    if (img->colorDepth != 8) {
        printf("Erreur : Ce programme prend uniquement les images BMP 8 bits.\n");
        free(img);
        fclose(file);
        return NULL;
    }

    // Lecture de la palette de couleurs (1024 octets pour 256 couleurs)
    fread(img->colorTable, sizeof(unsigned char), 1024, file);

    // Aller à l'offset des données
    unsigned int dataOffset = *(unsigned int *)&img->header[10];
    fseek(file, dataOffset, SEEK_SET);

    // Allocation et lecture des données de l'image
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

// === Fonction : bmp8_saveImage ===
// Paramètres :
//    - filename : chemin du fichier de sortie
//    - img : image à sauvegarder
// But :
//    - Écrire une image BMP 8 bits à partir de la structure t_bmp8
// Sortie :
//    - Aucun retour ; le fichier est écrit sur disque
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

// === Fonction : bmp8_free ===
// Paramètres :
//    - img : image à libérer
// But :
//    - Libérer la mémoire allouée dynamiquement à l'image
// Sortie :
//    - Aucun retour ; la mémoire est libérée
void bmp8_free(t_bmp8 *img) {
    if (img) {
        if (img->data) {
            free(img->data);
        }
        free(img);
    }
}

// === Fonction : bmp8_printInfo ===
// Paramètres :
//    - img : image à analyser
// But :
//    - Afficher les métadonnées de l’image (dimensions, profondeur, etc.)
// Sortie :
//    - Affichage des informations sur la console
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

// === Fonction : bmp8_negative ===
// Paramètres :
//    - img : image à transformer
// But :
//    - Appliquer un effet négatif à l’image (inverser les intensités)
// Sortie :
//    - Image modifiée directement
void bmp8_negative(t_bmp8 *img) {
    if (img) {
        for (unsigned int i = 0; i < img->dataSize; i++) {
            img->data[i] = 255 - img->data[i];
        }
    }
}

// === Fonction : bmp8_brightness ===
// Paramètres :
//    - img : image à éclaircir ou assombrir
//    - value : intensité à ajouter ou retirer (positive ou négative)
// But :
//    - Modifier la luminosité de chaque pixel
// Sortie :
//    - Image modifiée
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

// === Fonction : bmp8_threshold ===
// Paramètres :
//    - img : image à transformer
//    - threshold : valeur de seuil (0 à 255)
// But :
//    - Transformer l'image en noir et blanc selon un seuil
// Sortie :
//    - Image binaire (0 ou 255)
void bmp8_threshold(t_bmp8 *img, int threshold) {
    if (img) {
        for (unsigned int i = 0; i < img->dataSize; i++) {
            img->data[i] = (img->data[i] >= threshold) ? 255 : 0;
        }
    }
}

// === Fonction : bmp8_applyFilter ===
// Paramètres :
//    - img : image à filtrer
//    - kernel : matrice de convolution (noyau)
//    - kernelSize : taille du noyau (doit être impair)
// But :
//    - Appliquer un filtre par convolution (ex : flou, détection de contours)
// Sortie :
//    - Image modifiée avec le filtre appliqué
void bmp8_applyFilter(t_bmp8 *img, float **kernel, int kernelSize) {
    if (!img || !kernel) return;

    int offset = kernelSize / 2;
    unsigned char *newData = (unsigned char *)malloc(img->dataSize);
    if (!newData) {
        printf("Erreur : Allocation mémoire échouée pour le filtrage.\n");
        return;
    }

    // Parcours des pixels internes (on ignore les bords)
    for (unsigned int y = offset; y < img->height - offset; y++) {
        for (unsigned int x = offset; x < img->width - offset; x++) {
            float sum = 0.0f;

            // Application du noyau de convolution
            for (int ky = -offset; ky <= offset; ky++) {
                for (int kx = -offset; kx <= offset; kx++) {
                    int px = x + kx;
                    int py = y + ky;
                    sum += img->data[py * img->width + px] * kernel[ky + offset][kx + offset];
                }
            }

            // Limiter le résultat entre 0 et 255
            int pixel = (int)roundf(sum);
            if (pixel > 255) pixel = 255;
            if (pixel < 0) pixel = 0;
            newData[y * img->width + x] = (unsigned char)pixel;
        }
    }

    // Copier les données filtrées
    for (unsigned int y = offset; y < img->height - offset; y++) {
        for (unsigned int x = offset; x < img->width - offset; x++) {
            img->data[y * img->width + x] = newData[y * img->width + x];
        }
    }

    free(newData);
}

// === Fonction : bmp8_equalizeHistogram ===
// Paramètres :
//    - img : image à traiter
// But :
//    - Égaliser l'histogramme (répartir les niveaux de gris pour améliorer le contraste)
// Sortie :
//    - Image modifiée avec contraste amélioré
void bmp8_equalizeHistogram(t_bmp8 *img) {
    if (!img || !img->data) return;

    int histogram[256] = {0};

    // Étape 1 : Calcul de l'histogramme
    for (unsigned int i = 0; i < img->dataSize; i++) {
        histogram[img->data[i]]++;
    }

    // Étape 2 : Calcul de la fonction de répartition cumulative (CDF)
    float cdf[256] = {0};
    cdf[0] = histogram[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + histogram[i];
    }

    // Trouver la première valeur non nulle de la CDF (cdf_min)
    float cdf_min = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf[i] != 0) {
            cdf_min = cdf[i];
            break;
        }
    }

    // Étape 3 : Création de la table de correspondance (mapping)
    float scale = 255.0f / (img->width * img->height - cdf_min);
    unsigned char equalized[256];
    for (int i = 0; i < 256; i++) {
        equalized[i] = (unsigned char)(fmaxf(0, roundf((cdf[i] - cdf_min) * scale)));
    }

    // Étape 4 : Appliquer la table de correspondance
    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = equalized[img->data[i]];
    }
}
