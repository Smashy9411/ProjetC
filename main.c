#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"

void afficherInfosImage(t_bmp8 *image) {
    bmp8_printInfo(image); // on utilise directement la fonction du .c
}

void afficherMenuPrincipal() {
    printf("\n--- Menu principal ---\n");
    printf("1. Ouvrir une image\n");
    printf("2. Sauvegarder une image\n");
    printf("3. Appliquer un filtre\n");
    printf("4. Afficher les informations de l'image\n");
    printf("5. Quitter\n");
    printf(">>> Votre choix : ");
}

void afficherMenuFiltres() {
    printf("\n--- Menu des filtres ---\n");
    printf("1. Négatif (inversion des couleurs)\n");
    printf("2. Modifier la luminosité\n");
    printf("3. Binarisation (seuillage)\n");
    printf("4. Flou simple (box blur)\n");
    printf("5. Flou gaussien\n");
    printf("6. Détection de contours\n");
    printf("7. Effet de relief (emboss)\n");
    printf("8. Netteté (sharpen)\n");
    printf("9. Retour au menu principal\n");
    printf(">>> Votre choix : ");
}

// Fonctions pour créer les noyaux (kernels) de filtrage
float** creerKernelBoxBlur() {
    float **kernel = (float**)malloc(3 * sizeof(float*));
    for (int i = 0; i < 3; i++) {
        kernel[i] = (float*)malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = 1.0f / 9.0f;
        }
    }
    return kernel;
}

float** creerKernelGaussianBlur() {
    float **kernel = (float**)malloc(3 * sizeof(float*));
    int values[3][3] = {{1,2,1},{2,4,2},{1,2,1}};
    for (int i = 0; i < 3; i++) {
        kernel[i] = (float*)malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = values[i][j] / 16.0f;
        }
    }
    return kernel;
}

float** creerKernelOutline() {
    float **kernel = (float**)malloc(3 * sizeof(float*));
    int values[3][3] = {{-1,-1,-1},{-1,8,-1},{-1,-1,-1}};
    for (int i = 0; i < 3; i++) {
        kernel[i] = (float*)malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = (float)values[i][j];
        }
    }
    return kernel;
}

float** creerKernelEmboss() {
    float **kernel = (float**)malloc(3 * sizeof(float*));
    int values[3][3] = {{-2,-1,0},{-1,1,1},{0,1,2}};
    for (int i = 0; i < 3; i++) {
        kernel[i] = (float*)malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = (float)values[i][j];
        }
    }
    return kernel;
}

float** creerKernelSharpen() {
    float **kernel = (float**)malloc(3 * sizeof(float*));
    int values[3][3] = {{0,-1,0},{-1,5,-1},{0,-1,0}};
    for (int i = 0; i < 3; i++) {
        kernel[i] = (float*)malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = (float)values[i][j];
        }
    }
    return kernel;
}

void libererKernel(float **kernel, int size) {
    for (int i = 0; i < size; i++) {
        free(kernel[i]);
    }
    free(kernel);
}

int main(void) {
    t_bmp8 *image = NULL;
    int choice;
    char filename[256];

    while (1) {
        afficherMenuPrincipal();
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Entrez le nom du fichier BMP à ouvrir : ");
                scanf("%s", filename);
                if (image) {
                    bmp8_free(image);
                }
                image = bmp8_loadImage(filename);
                if (image) {
                    printf("Image chargée avec succès !\n");
                } else {
                    printf("Échec du chargement de l'image.\n");
                }
                break;

            case 2:
                if (image) {
                    printf("Entrez le nom du fichier BMP de sortie : ");
                    scanf("%s", filename);
                    bmp8_saveImage(filename, image);
                    printf("Image sauvegardée avec succès !\n");
                } else {
                    printf("Aucune image à sauvegarder.\n");
                }
                break;

            case 3:
                if (image) {
                    int filterChoice;
                    afficherMenuFiltres();
                    scanf("%d", &filterChoice);

                    switch (filterChoice) {
                        case 1:
                            bmp8_negative(image);
                            printf("Filtre négatif appliqué.\n");
                            break;
                        case 2: {
                            int brightness;
                            printf("Entrez la valeur de luminosité (-255 à 255) : ");
                            scanf("%d", &brightness);
                            bmp8_brightness(image, brightness);
                            printf("Luminosité modifiée.\n");
                            break;
                        }
                        case 3: {
                            int threshold;
                            printf("Entrez le seuil de binarisation (0 à 255) : ");
                            scanf("%d", &threshold);
                            bmp8_threshold(image, threshold);
                            printf("Binarisation effectuée.\n");
                            break;
                        }
                        case 4: {
                            float **kernel = creerKernelBoxBlur();
                            bmp8_applyFilter(image, kernel, 3);
                            libererKernel(kernel, 3);
                            printf("Filtre box blur appliqué.\n");
                            break;
                        }
                        case 5: {
                            float **kernel = creerKernelGaussianBlur();
                            bmp8_applyFilter(image, kernel, 3);
                            libererKernel(kernel, 3);
                            printf("Filtre flou gaussien appliqué.\n");
                            break;
                        }
                        case 6: {
                            float **kernel = creerKernelOutline();
                            bmp8_applyFilter(image, kernel, 3);
                            libererKernel(kernel, 3);
                            printf("Détection de contours appliquée.\n");
                            break;
                        }
                        case 7: {
                            float **kernel = creerKernelEmboss();
                            bmp8_applyFilter(image, kernel, 3);
                            libererKernel(kernel, 3);
                            printf("Effet relief appliqué.\n");
                            break;
                        }
                        case 8: {
                            float **kernel = creerKernelSharpen();
                            bmp8_applyFilter(image, kernel, 3);
                            libererKernel(kernel, 3);
                            printf("Netteté améliorée.\n");
                            break;
                        }
                        case 9:
                            printf("Retour au menu principal.\n");
                            break;
                        default:
                            printf("Filtre invalide.\n");
                            break;
                    }
                } else {
                    printf("Aucune image chargée.\n");
                }
                break;

            case 4:
                afficherInfosImage(image);
                break;

            case 5:
                printf("Au revoir !\n");
                if (image) {
                    bmp8_free(image);
                }
                return 0;

            default:
                printf("Option invalide, veuillez réessayer.\n");
        }
    }

    return 0;
}
