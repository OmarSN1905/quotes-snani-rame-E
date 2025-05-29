#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmp_8/bmp_8.h"
#include "bmp_24/bmp_24.h"
#include "histogramme/histogramme.h"

int main(void) {
    int choice, subchoice;
    char path[256];
    char outpath[256];
    t_bmp8  *img8  = NULL;
    t_bmp24 *img24 = NULL;

    while (1) {
        // Menu principal
        printf("\nVeuillez choisir une option :\n");
        printf(" 1. Ouvrir une image\n");
        printf(" 2. Appliquer un filtre\n");
        printf(" 3. Sauvegarder une image\n");
        printf(" 4. Afficher les informations de l'image\n");
        printf(" 5. Quitter\n");
        printf(">>> Votre choix : ");
        if (scanf("%d", &choice) != 1) break;

        switch (choice) {
            case 1:
                // Ouvrir une image
                printf("Chemin du fichier : ");
                scanf("%255s", path);
                if (strstr(path, "_gray.bmp")) {
                    img8 = bmp8_loadImage(path);
                    if (img8) printf("Image 8-bits chargee avec succes !\n");
                } else {
                    img24 = bmp24_loadImage(path);
                    if (img24) printf("Image 24-bits chargee avec succes !\n");
                }
                break;

            case 3:
                // Sauvegarder l'image
                printf("Chemin de sortie : ");
                scanf("%255s", outpath);
                if (img8) {
                    bmp8_saveImage(outpath, img8);
                    printf("Image 8-bits sauvegardee !\n");
                } else if (img24) {
                    bmp24_saveImage(img24, outpath);
                    printf("Image 24-bits sauvegardee !\n");
                } else {
                    printf("Aucune image à sauvegarder.\n");
                }
                break;

            case 2:
                // Appliquer un filtre
                printf("\nAppliquer un filtre :\n");
                printf(" 1. Negatif\n");
                printf(" 2. Luminosite (choisir valeur)\n");
                printf(" 3. Seuil (8 bits, choisir seuil)\n");
                printf(" 4. Box Blur\n");
                printf(" 5. Gaussian Blur\n");
                printf(" 6. Outline\n");
                printf(" 7. Emboss\n");
                printf(" 8. Sharpen\n");
                printf(" 9. Egalisation histogramme\n");
                printf("10. Retour\n");
                printf(">>> Votre choix : ");
                if (scanf("%d", &subchoice) != 1) break;

                if (img8) {
                    switch (subchoice) {
                        case 1: // Négatif
                            bmp8_negative(img8);
                            break;
                        case 2: { // Luminosité
                            int val;
                            printf("Valeur de luminosite : ");
                            scanf("%d", &val);
                            bmp8_brightness(img8, val);
                        } break;
                        case 3: { // Seuillage
                            int thr;
                            printf("Seuil de binarisation (0-255) : ");
                            scanf("%d", &thr);
                            bmp8_threshold(img8, thr);
                        } break;
                        case 4: { // Box Blur 3x3
                            float *k[3]; static float data[3][3] = {
                                {1/9.f,1/9.f,1/9.f},
                                {1/9.f,1/9.f,1/9.f},
                                {1/9.f,1/9.f,1/9.f}
                            };
                            for (int i = 0; i < 3; i++) k[i] = data[i];
                            bmp8_applyFilter(img8, k, 3);
                        } break;
                        case 5: { // Gaussian Blur 3x3
                            float *k[3]; static float data[3][3] = {
                                {1/16.f,2/16.f,1/16.f},
                                {2/16.f,4/16.f,2/16.f},
                                {1/16.f,2/16.f,1/16.f}
                            };
                            for (int i = 0; i < 3; i++) k[i] = data[i];
                            bmp8_applyFilter(img8, k, 3);
                        } break;
                        case 6: { // Outline
                            float *k[3]; static float data[3][3] = {
                                {-1,-1,-1},
                                {-1, 8,-1},
                                {-1,-1,-1}
                            };
                            for (int i = 0; i < 3; i++) k[i] = data[i];
                            bmp8_applyFilter(img8, k, 3);
                        } break;
                        case 7: { // Emboss
                            float *k[3]; static float data[3][3] = {
                                {-2,-1,0},
                                {-1, 1,1},
                                { 0, 1,2}
                            };
                            for (int i = 0; i < 3; i++) k[i] = data[i];
                            bmp8_applyFilter(img8, k, 3);
                        } break;
                        case 8: { // Sharpen
                            float *k[3]; static float data[3][3] = {
                                { 0,-1, 0},
                                {-1, 5,-1},
                                { 0,-1, 0}
                            };
                            for (int i = 0; i < 3; i++) k[i] = data[i];
                            bmp8_applyFilter(img8, k, 3);
                        } break;
                        case 9: { // Égalisation
                            unsigned int *h = bmp8_computeHistogram(img8);
                            unsigned int *m = bmp8_computeCDF(h);
                            free(h);
                            bmp8_equalize(img8, m);
                            free(m);
                        } break;
                        default:
                            break;
                    }
                    printf("Filtre applique sur image 8-bits.\n");
                } else if (img24) {
                    switch (subchoice) {
                        case 1: // Négatif
                            bmp24_negative(img24);
                            break;
                        case 2: { // Luminosité
                            int val;
                            printf("Valeur de luminosite : ");
                            scanf("%d", &val);
                            bmp24_brightness(img24, val);
                        } break;
                        case 3: // Niveaux de gris
                            bmp24_grayscale(img24);
                            break;
                        case 4: // Box Blur
                            bmp24_boxBlur(img24);
                            break;
                        case 5: // Gaussian Blur
                            bmp24_gaussianBlur(img24);
                            break;
                        case 6: // Outline
                            bmp24_outline(img24);
                            break;
                        case 7: // Emboss
                            bmp24_emboss(img24);
                            break;
                        case 8: // Sharpen
                            bmp24_sharpen(img24);
                            break;
                        case 9: // Égalisation
                            bmp24_equalize(img24);
                            break;
                        default:
                            break;
                    }
                    printf("Filtre applique sur image 24-bits.\n");
                } else {
                    printf("Aucune image chargee.\n");
                }
                break;

            case 4:
                // Afficher les infos de l'image
                if (img8) {
                    bmp8_printInfo(img8);
                } else if (img24) {
                    // Affiche manuellement les infos pour image 24 bits
                    printf("Image Info:\n");
                    printf(" Width      : %d px\n", img24->width);
                    printf(" Height     : %d px\n", img24->height);
                    printf(" Color Depth: %d bits\n", img24->colorDepth);
                    printf(" Data Size  : %d bytes\n", img24->width * img24->height * 3);
                } else {
                    printf("Aucune image charge.\n");
                }
                break;

            case 5:
                // Quitter et libérer la mémoire
                if (img8)  bmp8_free(img8);
                if (img24) bmp24_free(img24);
                return 0;

            default:
                printf("Choix invalide.\n");
        }
    }

    return 0;
}