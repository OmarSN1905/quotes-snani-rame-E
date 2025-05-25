// main.c — test complet d’un filtre (ici « flou gaussien » 3×3)
#include <stdio.h>
#include <stdlib.h>
#include "bmp_8/bmp_8.h"
#include "histogramme/histogramme.h"


int main(void) {
    // Les deux images 8-bits à traiter :
    const char *inputs[]  = {
        "../image_bmp/barbara_gray.bmp",
        "../image_bmp/test_gaussian.bmp"
    };
    // Les chemins de sortie correspondants :
    const char *outputs[] = {
        "../image_bmp/barbara_gray_eq.bmp",
        "../image_bmp/test_gaussian_eq.bmp"
    };

    for (int i = 0; i < 2; ++i) {
        const char *in  = inputs[i];
        const char *out = outputs[i];
        printf("Traitement de %s → %s\n", in, out);

        // 1) Chargement
        t_bmp8 *img = bmp8_loadImage(in);
        if (!img) {
            fprintf(stderr, "Erreur : impossible de charger %s\n", in);
            continue;
        }

        // 2) Histogramme brut
        unsigned int *hist = bmp8_computeHistogram(img);
        if (!hist) {
            bmp8_free(img);
            continue;
        }

        // 3) CDF normalisée
        unsigned int *hist_eq = bmp8_computeCDF(hist);
        free(hist);
        if (!hist_eq) {
            bmp8_free(img);
            continue;
        }

        // 4) Égalisation
        bmp8_equalize(img, hist_eq);
        free(hist_eq);

        // 5) Sauvegarde du résultat
        bmp8_saveImage(out, img);

        // 6) Libération mémoire
        bmp8_free(img);
    }

    // On termine avec code de succès "1" comme demandé
    return 0;
}








































/*
int main(void) {
    const char* in  = "../image_bmp/barbara_gray.bmp";
    const char* out = "../image_bmp/test_gaussian.bmp";

    t_bmp8* img = bmp8_loadImage(in);
    if (!img) {
        fprintf(stderr, "Impossible de charger %s\n", in);
        return 1;
    }

    //--- Noyau de flou gaussien 3×3
    float G[3][3] = {
        {1/16.0f, 2/16.0f, 1/16.0f},
        {2/16.0f, 4/16.0f, 2/16.0f},
        {1/16.0f, 2/16.0f, 1/16.0f}
    };
    float* kernel[3] = { G[0], G[1], G[2] };

    // Application du filtre
    //bmp8_applyFilter(img, kernel, 3);


    bmp8_negative(img);

    // Sauvegarde et infos
    bmp8_saveImage(out, img);
    printf("Flou gaussien appliqué et sauvegardé dans : %s\n", out);
    bmp8_printInfo(img);

    bmp8_free(img);
    return 0;
}
*/