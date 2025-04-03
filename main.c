#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"

void afficherInfosImage(t_bmp8 *image) {
    if (image) {
        printf("\n--- Informations de l'image ---\n");
        printf("Dimensions : %dx%d\n", image->width, image->height);
        printf("Profondeur de couleur : %d bits\n", image->colorDepth);
        printf("Taille des données : %d octets\n", image->dataSize);
    } else {
        printf("Aucune image chargée.\n");
    }
}

int main(void) {
    t_bmp8 *image = NULL;
    int choice;
    char filename[256];

    while (1) {
        printf("\nVeuillez choisir une option :\n");
        printf(" 1. Ouvrir une image\n");
        printf(" 2. Sauvegarder une image\n");
        printf(" 3. Appliquer un filtre (inversion des couleurs)\n");
        printf(" 4. Afficher les informations de l'image\n");
        printf(" 5. Quitter\n");
        printf(">>> Votre choix : ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Entrez le nom du fichier BMP à ouvrir : ");
                scanf("%s", filename);
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
                    bmp8_invertColors(image);
                    printf("Filtre !\n");
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
                    free(image->data);
                    free(image);
                }
                return 0;

            default:
                printf("Option invalide, veuillez réessayer.\n");
        }
    }

    return 0;
}
