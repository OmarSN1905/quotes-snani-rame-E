#include "bmp_8.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * Charge une image BMP 8 bits depuis un fichier
 * Lit l'en-tête, la table de couleurs et les données pixel
 */
t_bmp8* bmp8_loadImage(const char* filename) {
    FILE* f = fopen(filename, "rb");  // Ouvre le fichier en lecture binaire
    if (!f) {
        perror("Erreur lors de l'ouverture du fichier");
        return NULL;
    }

    t_bmp8* img = malloc(sizeof(t_bmp8));  // Alloue la structure image
    if (!img) {
        perror("Erreur d'allocation mémoire pour t_bmp8");
        fclose(f);
        return NULL;
    }

    // Lecture de l'en-tête BMP (54 octets) et de la table de couleurs (1024 octets)
    if (fread(img->header,     1, 54,   f) != 54 ||
        fread(img->colorTable, 1, 1024, f) != 1024) {
        fprintf(stderr, "Erreur de lecture de l'en-tête\n");
        free(img);
        fclose(f);
        return NULL;
    }

    // Extraction des métadonnées depuis l'en-tête BMP
    img->width      = *(unsigned int*)&img->header[18];  // Largeur
    img->height     = *(unsigned int*)&img->header[22];  // Hauteur
    img->colorDepth = *(unsigned int*)&img->header[28];  // Profondeur (doit être 8)
    img->dataSize   = *(unsigned int*)&img->header[34];  // Taille des données

    // Vérifie que l'image est bien en 8 bits
    if (img->colorDepth != 8) {
        fprintf(stderr, "Erreur : profondeur non supportée (%u bits)\n", img->colorDepth);
        free(img);
        fclose(f);
        return NULL;
    }

    // Si la taille des données n'est pas indiquée dans l'en-tête, on la calcule
    if (img->dataSize == 0)
        img->dataSize = img->width * img->height;

    // Allocation mémoire pour les données pixel
    img->data = malloc(img->dataSize);
    if (!img->data) {
        perror("Erreur d'allocation pour les données");
        free(img);
        fclose(f);
        return NULL;
    }

    // Lecture des données pixel
    if (fread(img->data, 1, img->dataSize, f) != img->dataSize) {
        fprintf(stderr, "Erreur de lecture des données pixel\n");
        free(img->data);
        free(img);
        fclose(f);
        return NULL;
    }

    fclose(f);  // Fermeture du fichier
    return img; // Retourne l'image chargée
}

/**
 * Sauvegarde une image BMP 8 bits vers un fichier
 */
void bmp8_saveImage(const char* filename, t_bmp8* img) {
    FILE* f = fopen(filename, "wb");  // Ouvre en écriture binaire
    if (!f) {
        perror("Erreur à l'ouverture en écriture");
        return;
    }

    // Écrit l'en-tête, la palette de couleurs, puis les données pixel
    fwrite(img->header,     1, 54,    f);
    fwrite(img->colorTable, 1, 1024,  f);
    fwrite(img->data,       1, img->dataSize, f);
    fclose(f);
}

/**
 * Libère la mémoire occupée par une image BMP 8 bits
 */
void bmp8_free(t_bmp8* img) {
    if (!img) return;
    free(img->data);
    free(img);
}

/**
 * Affiche les informations principales d'une image BMP 8 bits
 */
void bmp8_printInfo(t_bmp8* img) {
    if (!img) {
        printf("Aucune image chargée.\n");
        return;
    }
    printf("Image Info:\n");
    printf(" Width      : %u px\n", img->width);
    printf(" Height     : %u px\n", img->height);
    printf(" Color Depth: %u bits\n", img->colorDepth);
    printf(" Data Size  : %u bytes\n", img->dataSize);
}

/**
 * Applique un négatif à l’image (255 - valeur) pixel par pixel
 */
void bmp8_negative(t_bmp8* img) {
    unsigned int n = img->width * img->height;
    for (unsigned int i = 0; i < n; i++)
        img->data[i] = 255 - img->data[i];  // Inversion de la valeur
}

/**
 * Applique un ajustement de luminosité sur chaque pixel
 * value > 0 : éclaircit ; value < 0 : assombrit
 */
void bmp8_brightness(t_bmp8* img, int value) {
    unsigned int n = img->width * img->height;
    for (unsigned int i = 0; i < n; i++) {
        int v = img->data[i] + value;  // Ajout du décalage
        if (v <  0)   v = 0;           // Clamp minimum
        if (v > 255) v = 255;          // Clamp maximum
        img->data[i] = (unsigned char)v;
    }
}

/**
 * Applique un seuillage : pixels >= seuil deviennent blancs (255), les autres noirs (0)
 */
void bmp8_threshold(t_bmp8* img, int threshold) {
    unsigned int n = img->width * img->height;
    for (unsigned int i = 0; i < n; i++)
        img->data[i] = (img->data[i] >= threshold) ? 255 : 0;
}

/**
 * Applique un filtre convolutif à l’image (filtre linéaire)
 * kernel : matrice du filtre (taille impaire, ex. 3×3, 5×5)
 */
void bmp8_applyFilter(t_bmp8* img, float** kernel, int kernelSize) {
    int  w = img->width, h = img->height;
    int  r = kernelSize / 2;  // Rayon du noyau (ex: 3x3 → r=1)

    // Copie des données sources (pour ne pas modifier pendant la convolution)
    unsigned char* src = malloc(img->dataSize);
    if (!src) return;
    memcpy(src, img->data, img->dataSize);

    // Application du filtre convolution (ignore les bords)
    for (int y = r; y < h - r; y++) {
        for (int x = r; x < w - r; x++) {
            float sum = 0.0f;
            for (int ky = -r; ky <= r; ky++) {
                for (int kx = -r; kx <= r; kx++) {
                    // Valeur du pixel voisin
                    unsigned char p = src[(y + ky) * w + (x + kx)];
                    // Poids associé dans le noyau
                    sum += p * kernel[ky + r][kx + r];
                }
            }

            // Clamp la somme entre 0 et 255
            if (sum < 0)   sum = 0;
            if (sum > 255) sum = 255;

            // Mise à jour du pixel filtré
            img->data[y * w + x] = (unsigned char)sum;
        }
    }

    free(src);  // Libère la mémoire temporaire
}
