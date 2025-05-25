#include <stdlib.h>
#include <string.h>
#include "bmp8.h"
#include "bmp24.h"


// --- MENUS ---

void afficherMenuPrincipal() {
    printf("\n--- Menu principal ---\n");
    printf("1. Ouvrir une image\n");
    printf("2. Sauvegarder une image\n");
    printf("3. Appliquer un filtre\n");
    printf("4. Afficher les informations de l'image\n");
    printf("5. Quitter\n");
    printf(">>> Votre choix : ");
}

void afficherMenuFiltres8bits() {
    printf("\n--- Filtres pour image 8 bits ---\n");
    printf("1. Negatif\n");
    printf("2. Modifier la luminosite\n");
    printf("3. Binarisation (seuillage)\n");
    printf("4. Box blur\n");
    printf("5. Flou gaussien\n");
    printf("6. Contours\n");
    printf("7. Relief\n");
    printf("8. Nettete\n");
    printf("9. Egalisation histogramme\n");
    printf("10. Retour\n");
    printf(">>> ");
}



void afficherMenuFiltres24bits() {
    printf("\n--- Filtres pour image 24 bits ---\n");
    printf("1. Negatif\n");
    printf("2. Niveaux de gris\n");
    printf("3. Modifier la luminosite\n");
    printf("4. Filtres convolution\n");
    printf("5. Egalisation histogramme\n");
    printf("6. Retour\n");
    printf(">>> ");
}



void afficherMenuFiltresConvolution24bits() {
    printf("\n--- Filtres de convolution 24 bits ---\n");
    printf("1. Box blur\n");
    printf("2. Flou gaussien\n");
    printf("3. Contours (outline)\n");
    printf("4. Relief (emboss)\n");
    printf("5. Nettete (sharpen)\n");
    printf("6. Retour\n");
    printf(">>> ");
}


// --- KERNELS ---

float** creerKernel(int values[3][3], float diviseur) {
    float **kernel = (float**)malloc(3 * sizeof(float*));
    for (int i = 0; i < 3; i++) {
        kernel[i] = (float*)malloc(3 * sizeof(float));
        for (int j = 0; j < 3; j++) {
            kernel[i][j] = values[i][j] / diviseur;
        }
    }
    return kernel;
}

void libererKernel(float **kernel, int size) {
    for (int i = 0; i < size; i++) free(kernel[i]);
    free(kernel);
}

// --- MAIN ---

int main(void) {
    t_bmp8 *image8 = NULL;
    t_bmp24 *image24 = NULL;
    int choice;
    char filename[256];

    while (1) {
        afficherMenuPrincipal();
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                int format;
                printf("Quel type d'image voulez-vous charger ? (1 = BMP 8 bits, 2 = BMP 24 bits) : ");
                scanf("%d", &format);
                printf("Entrez le nom du fichier : ");
                scanf("%s", filename);

                if (image8) bmp8_free(image8);
                if (image24) bmp24_free(image24);

                if (format == 1) {
                    image8 = bmp8_loadImage(filename);
                    if (image8) printf("Image 8 bits chargee avec succes !\n");
                    else printf("Echec du chargement.\n");
                } else if (format == 2) {
                    image24 = bmp24_loadImage(filename);
                    if (image24) printf("Image 24 bits chargee avec succes !\n");
                    else printf("Echec du chargement.\n");
                } else {
                    printf("Format inconnu.\n");
                }
                break;
            }

            case 2:
                printf("Entrez le nom du fichier de sortie : ");
                scanf("%s", filename);
                if (image8) {
                    bmp8_saveImage(filename, image8);
                    printf("Image 8 bits sauvegardee !\n");
                } else if (image24) {
                    bmp24_saveImage(image24, filename);
                    printf("Image 24 bits sauvegardee !\n");
                } else {
                    printf("Aucune image à sauvegarder.\n");
                }
                break;

            case 3:
                if (image8) {
                    int f;
                    afficherMenuFiltres8bits();
                    scanf("%d", &f);
                    switch (f) {
                        case 1: bmp8_negative(image8); break;
                        case 2: {
                            int b;
                            printf("Luminosite (-255 à 255) : "); scanf("%d", &b);
                            bmp8_brightness(image8, b);
                            break;
                        }
                        case 3: {
                            int t;
                            printf("Seuil (0 à 255) : "); scanf("%d", &t);
                            bmp8_threshold(image8, t);
                            break;
                        }
                        case 4: {
                            int k[3][3] = {{1,1,1},{1,1,1},{1,1,1}};
                            float **kernel = creerKernel(k, 9.0f);
                            bmp8_applyFilter(image8, kernel, 3);
                            libererKernel(kernel, 3);
                            break;
                        }
                        case 5: {
                            int k[3][3] = {{1,2,1},{2,4,2},{1,2,1}};
                            float **kernel = creerKernel(k, 16.0f);
                            bmp8_applyFilter(image8, kernel, 3);
                            libererKernel(kernel, 3);
                            break;
                        }
                        case 6: {
                            int k[3][3] = {{-1,-1,-1},{-1,8,-1},{-1,-1,-1}};
                            float **kernel = creerKernel(k, 1.0f);
                            bmp8_applyFilter(image8, kernel, 3);
                            libererKernel(kernel, 3);
                            break;
                        }
                        case 7: {
                            int k[3][3] = {{-2,-1,0},{-1,1,1},{0,1,2}};
                            float **kernel = creerKernel(k, 1.0f);
                            bmp8_applyFilter(image8, kernel, 3);
                            libererKernel(kernel, 3);
                            break;
                        }
                        case 8: {
                            int k[3][3] = {{0,-1,0},{-1,5,-1},{0,-1,0}};
                            float **kernel = creerKernel(k, 1.0f);
                            bmp8_applyFilter(image8, kernel, 3);
                            libererKernel(kernel, 3);
                            break;
                        }
                        case 9: bmp8_equalizeHistogram(image8); break;
                        case 10: break;
                        default: printf("Filtre invalide.\n");
                    }
                    printf("Filtre 8 bits applique.\n");
                } else if (image24) {
                    int f;
                    afficherMenuFiltres24bits();
                    scanf("%d", &f);
                    switch (f) {
                        case 1: bmp24_negative(image24); break;
                        case 2: bmp24_grayscale(image24); break;
                        case 3: {
                            int b;
                            printf("Luminosite (-255 à 255) : "); scanf("%d", &b);
                            bmp24_brightness(image24, b);
                            break;
                        }
                        case 4: {
                            int fc;
                            afficherMenuFiltresConvolution24bits();
                            scanf("%d", &fc);
                            switch (fc) {
                                case 1: bmp24_boxBlur(image24); break;
                                case 2: bmp24_gaussianBlur(image24); break;
                                case 3: bmp24_outline(image24); break;
                                case 4: bmp24_emboss(image24); break;
                                case 5: bmp24_sharpen(image24); break;
                                case 6: break;
                                default: printf("Filtre invalide.\n"); break;
                            }
                            break;
                        }
                        case 5: bmp24_equalizeHistogram(image24); break;
                        case 6: break;
                        default: printf("Filtre invalide.\n"); break;
                    }
                    printf("Filtre 24 bits appliqué.\n");

                } else {
                    printf("Aucune image chargée.\n");
                }
                break;

            case 4:
                if (image8) bmp8_printInfo(image8);
                else if (image24) {
                    printf("\n--- Informations image 24 bits ---\n");
                    printf("Dimensions : %dx%d\n", image24->width, image24->height);
                    printf("Profondeur : %d bits\n", image24->colorDepth);
                } else {
                    printf("Aucune image chargée.\n");
                }
                break;

            case 5:
                if (image8) bmp8_free(image8);
                if (image24) bmp24_free(image24);
                printf("Au revoir !\n");
                return 0;

            default:
                printf("Option invalide.\n");
        }
    }
}
