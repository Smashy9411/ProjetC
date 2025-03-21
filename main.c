#include <stdio.h>
#include <stdlib.h>
#include "bmp8.h"
#

int main(void) {
    int choice;
    printf("Veuillez choisir une option :\n 1.  Ouvrir une image \n 2.  Sauvegarder une image\n 3.  Appliquer un filtre\n 4.  Afficher les informations de l'image \n 5.  Quitter\n ");
    scanf("%d",&choice);



    if (choice == 1) {
printf(">>> Votre choix : 1\n");

    }
    if (choice == 2) {
        printf(">>> Votre choix : 2\n");
    }

    if (choice == 3) {
        printf(">>> Votre choix : 3\n");

    }

    if (choice == 4) {
        printf(">>> Votre choix : 4\n");
    }
    if (choice == 5) {
        return 0;
    }
    return 0;
}