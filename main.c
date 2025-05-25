#include <stdio.h>
#include "bmp_24.h"
#include <string.h>


//bibliothèue que t as besoin dans le main

// Fonction pour afficher les informations de l'image
void bmp24_printInfo(t_bmp24 *img) {
    if (!img) {
        printf("Image non chargée.\n");
        return;
    }
    printf("\nInformations de l'image :\n");
    printf("Largeur : %d pixels\n", img->width);
    printf("Hauteur : %d pixels\n", img->height);
    printf("Profondeur de couleur : %d bits\n", img->colorDepth);
}

int main() {

    t_bmp24 *image = NULL;
    int run = 1;
    int choice, filterChoice, brightnessValue;
    char filename[256];
    char saveFilename[256];

    // Demander le nom de l'image à l'utilisateur
    printf("Entrez le nom de l'image BMP à charger (ex: mon_image.bmp) : ");
    fgets(filename, sizeof(filename), stdin);
    // Supprimer le retour à la ligne
    filename[strcspn(filename, "\n")] = 0;

    // Construire le chemin complet
    char fullPath[512];
    sprintf(fullPath, "image/%s", filename);

    printf("Tentative de chargement de : %s\n", fullPath);
    image = bmp24_loadImage(fullPath);

    if (!image) {
        printf("Erreur lors du chargement de l'image %s.\n", fullPath);
        printf("Veuillez vérifier que :\n");
        printf("1. Le fichier existe dans le dossier 'image'\n");
        printf("2. Le nom du fichier est exactement le même (attention aux espaces et majuscules)\n");
        printf("3. Le fichier est bien au format BMP\n");
        return -1;
    }

    printf("Image %s chargée avec succès !\n", fullPath);

    while (run) {
        printf("\n=== MENU TEST PARTIE 2 ===\n");
        printf("1. Afficher les informations de l'image\n");
        printf("2. Appliquer un filtre\n");
        printf("3. Sauvegarder l'image\n");
        printf("4. Quitter\n");
        printf(">>>>> Votre choix : ");
        scanf("%d", &choice);
        while (getchar() != '\n');

        switch (choice) {
            case 1:
                bmp24_printInfo(image);
                break;

            case 2:
                printf("\nChoisissez un filtre :\n");
                printf("1. Négatif\n");
                printf("2. Niveaux de gris\n");
                printf("3. Luminosité\n");
                printf("4. Flou moyen\n");
                printf("5. Flou gaussien\n");
                printf("6. Contours\n");
                printf("7. Relief\n");
                printf("8. Netteté\n");
                printf(">>>>> Votre choix : ");
                scanf("%d", &filterChoice);
                while (getchar() != '\n');

                // Demander le nom du fichier de sortie
                printf("Entrez le nom du fichier de sortie (ex: output.bmp) : ");
                fgets(saveFilename, sizeof(saveFilename), stdin);
                saveFilename[strcspn(saveFilename, "\n")] = 0;

                // Construire le chemin complet pour la sauvegarde
                char savePath[512];
                sprintf(savePath, "image/%s", saveFilename);

                switch (filterChoice) {
                    case 1:
                        bmp24_negative(image);
                        printf("Filtre négatif appliqué.\n");
                        break;
                    case 2:
                        bmp24_grayscale(image);
                        printf("Conversion en niveaux de gris appliquée.\n");
                        break;
                    case 3:
                        printf("Entrez la valeur de luminosité (positive ou négative) : ");
                        scanf("%d", &brightnessValue);
                        while (getchar() != '\n');
                        bmp24_brightness(image, brightnessValue);
                        printf("Filtre de luminosité appliqué.\n");
                        break;
                    case 4:
                        bmp24_boxBlur(image);
                        printf("Flou moyen appliqué.\n");
                        break;
                    case 5:
                        bmp24_gaussianBlur(image);
                        printf("Flou gaussien appliqué.\n");
                        break;
                    case 6:
                        bmp24_outline(image);
                        printf("Détection de contours appliquée.\n");
                        break;
                    case 7:
                        bmp24_emboss(image);
                        printf("Effet de relief appliqué.\n");
                        break;
                    case 8:
                        bmp24_sharpen(image);
                        printf("Netteté appliquée.\n");
                        break;
                    default:
                        printf("Choix invalide.\n");
                        break;
                }

                // Sauvegarder l'image avec le filtre appliqué
                bmp24_saveImage(image, savePath);
                printf("Image sauvegardée sous %s\n", savePath);
                break;

            case 3:
                printf("Entrez le nom du fichier de sauvegarde (ex: output.bmp) : ");
                fgets(saveFilename, sizeof(saveFilename), stdin);
                // Supprimer le retour à la ligne
                saveFilename[strcspn(saveFilename, "\n")] = 0;

                // Construire le chemin complet pour la sauvegarde
                char savePath2[512];
                sprintf(savePath2, "image/%s", saveFilename);

                bmp24_saveImage(image, savePath2);
                printf("Image sauvegardée sous %s.\n", savePath2);
                break;

            case 4:
                run = 0;
                break;

            default:
                printf("Choix invalide.\n");
        }
    }

    bmp24_free(image);
    printf("Programme terminé.\n");
    return 0;
}

// info plus : initi    liser les pointeur = NULL