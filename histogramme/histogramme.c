#include "histogramme.h"
#include "../bmp_8/bmp_8.h"
#include "../bmp_24/bmp_24.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/**
 * Calcule l'histogramme d'une image 8 bits.
 * Chaque case de `hist` compte le nombre d'occurrences de la valeur correspondante (0 à 255).
 */
unsigned int * bmp8_computeHistogram(t_bmp8 *img) {
    // Vérifie si le pointeur image ou ses données sont nulles
    if (!img || !img->data) {
        fprintf(stderr, "bmp8_computeHistogram: pointeur img invalide\n");
        return NULL;
    }

    // Alloue un tableau de 256 compteurs initialisés à zéro
    unsigned int *hist = calloc(256, sizeof *hist);
    if (!hist) {
        perror("bmp8_computeHistogram");
        return NULL;
    }

    // Parcourt tous les pixels de l'image et incrémente le compteur correspondant à leur valeur
    for (unsigned int i = 0; i < img->dataSize; i++)
        hist[ img->data[i] ]++;

    // Retourne l'histogramme
    return hist;
}

/**
 * Calcule la CDF (cumulative distribution function) à partir d'un histogramme
 * et retourne une table de remapping pour égalisation d'histogramme.
 */
unsigned int * bmp8_computeCDF(unsigned int * hist) {
    // Vérifie que le pointeur vers l'histogramme est valide
    if (!hist) {
        fprintf(stderr, "bmp8_computeCDF: pointeur hist invalide\n");
        return NULL;
    }

    // Allocation de la CDF et de la table de remapping
    unsigned int *cdf     = malloc(256 * sizeof *cdf);
    unsigned int *hist_eq = malloc(256 * sizeof *hist_eq);
    if (!cdf || !hist_eq) {
        fprintf(stderr, "bmp8_computeCDF: échec malloc\n");
        free(cdf);
        free(hist_eq);
        return NULL;
    }

    // 1. Construction de la CDF (somme cumulative des fréquences)
    unsigned int cum = 0;
    for (int i = 0; i < 256; i++) {
        cum    += hist[i];
        cdf[i] = cum;
    }

    // Total de pixels
    unsigned int N = cum;

    // 2. Trouver le premier cdf non nul (pour éviter les divisions par zéro)
    unsigned int cdfmin = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf[i]) {
            cdfmin = cdf[i];
            break;
        }
    }

    // 3. Création de la table de correspondance (mapping) normalisée sur [0,255]
    double denom = (double)(N - cdfmin);
    for (int i = 0; i < 256; i++) {
        double val = (denom <= 0.0)
            ? ((double)cdf[i] / N * 255.0)                 // Cas particulier (image uniforme)
            : (((double)(cdf[i] - cdfmin) / denom) * 255.0); // Cas normal

        hist_eq[i] = (unsigned int)round(val);  // Valeur entière arrondie
    }

    // Libère la CDF temporaire
    free(cdf);

    // Retourne la table de remapping
    return hist_eq;
}

/**
 * Applique une égalisation d'histogramme à une image 8 bits, selon une table de remapping fournie.
 */
void bmp8_equalize(t_bmp8 *img, unsigned int *hist_eq) {
    // Vérifie que les pointeurs sont valides
    if (!img || !img->data || !hist_eq) {
        fprintf(stderr, "bmp8_equalize: pointeur invalide\n");
        return;
    }

    // Applique la nouvelle valeur à chaque pixel en utilisant la table hist_eq
    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = (unsigned char)hist_eq[ img->data[i] ];
    }
}

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //

/**
 * Égalise une image 24 bits (RVB) en travaillant uniquement sur la luminance Y.
 * La conversion se fait via l'espace YUV pour préserver la teinte.
 */
void bmp24_equalize(t_bmp24 *img) {
    // Vérifie si l'image ou ses données sont nulles
    if (!img || !img->data) {
        fprintf(stderr, "bmp24_equalize: img invalide\n");
        return;
    }

    int w = img->width;
    int h = img->height;
    int N = w * h;

    // 1. Calcul de l'histogramme de la luminance (valeurs Y)
    unsigned int hist[256] = {0};
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            t_pixel p = img->data[y][x];
            int Y = (int)round(0.299 * p.red + 0.587 * p.green + 0.114 * p.blue);
            hist[Y]++;
        }
    }

    // 2. Calcul de la CDF cumulative
    unsigned int cdf[256];
    unsigned int cum = 0;
    for (int i = 0; i < 256; i++) {
        cum    += hist[i];
        cdf[i] = cum;
    }

    // 3. Détermination de cdfmin (premier niveau de luminance non nul)
    unsigned int cdfmin = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf[i] != 0) {
            cdfmin = cdf[i];
            break;
        }
    }

    // 4. Construction de la table de remapping pour Y
    unsigned char mapY[256];
    double denom = (double)(N - cdfmin);
    for (int i = 0; i < 256; i++) {
        double v;
        if (denom <= 0.0) {
            // Image uniforme : on utilise une normalisation simple
            v = ((double)cdf[i] / N) * 255.0;
        } else {
            // Normalisation standard entre cdfmin et le max
            v = ((double)(cdf[i] - cdfmin) / denom) * 255.0;
        }
        mapY[i] = (unsigned char)round(v);
    }

    // 5. Application de l’égalisation à chaque pixel (conversion YUV ↔ RGB)
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            t_pixel p = img->data[y][x];

            // Extraction des composantes RVB
            double R = p.red, G = p.green, B = p.blue;

            // Conversion RGB → YUV
            double Y  = 0.299 * R + 0.587 * G + 0.114 * B;
            double U  = (B - Y) * 0.565;
            double V  = (R - Y) * 0.713;

            // Remplacement de Y par la valeur égalisée
            int Y1 = mapY[(int)round(Y)];

            // Conversion YUV → RGB (inverse)
            int R1 = (int)round(Y1 + 1.403 * V);
            int G1 = (int)round(Y1 - 0.344 * U - 0.714 * V);
            int B1 = (int)round(Y1 + 1.773 * U);

            // Clamp des valeurs dans [0,255]
            if      (R1 < 0)   R1 = 0;   else if (R1 > 255) R1 = 255;
            if      (G1 < 0)   G1 = 0;   else if (G1 > 255) G1 = 255;
            if      (B1 < 0)   B1 = 0;   else if (B1 > 255) B1 = 255;

            // Écriture dans l'image du nouveau pixel égalisé
            img->data[y][x].red   = (unsigned char)R1;
            img->data[y][x].green = (unsigned char)G1;
            img->data[y][x].blue  = (unsigned char)B1;
        }
    }
}
