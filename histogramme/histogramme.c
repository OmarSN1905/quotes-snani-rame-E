#include "histogramme.h"
#include "../bmp_8/bmp8.h"
#include "../bmp_24/bmp24.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

unsigned int * bmp8_computeHistogram(t_bmp8 *img) {
    if (!img || !img->data) {
        fprintf(stderr, "bmp8_computeHistogram: pointeur img invalide\n");
        return NULL;
    }
    unsigned int *hist = calloc(256, sizeof *hist);
    if (!hist) {
        perror("bmp8_computeHistogram");
        return NULL;
    }
    for (unsigned int i = 0; i < img->dataSize; i++)
        hist[ img->data[i] ]++;
    return hist;
}

unsigned int * bmp8_computeCDF(unsigned int * hist) {
    if (!hist) {
        fprintf(stderr, "bmp8_computeCDF: pointeur hist invalide\n");
        return NULL;
    }
    unsigned int *cdf    = malloc(256 * sizeof *cdf);
    unsigned int *hist_eq = malloc(256 * sizeof *hist_eq);
    if (!cdf || !hist_eq) {
        fprintf(stderr, "bmp8_computeCDF: échec malloc\n");
        free(cdf);
        free(hist_eq);
        return NULL;
    }
    unsigned int cum = 0;
    for (int i = 0; i < 256; i++) {
        cum    += hist[i];
        cdf[i] = cum;
    }
    unsigned int N = cum;
    unsigned int cdfmin = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf[i]) { cdfmin = cdf[i]; break; }
    }
    double denom = (double)(N - cdfmin);
    for (int i = 0; i < 256; i++) {
        double val = (denom <= 0.0)
          ? ((double)cdf[i] / N * 255.0)
          : (((double)(cdf[i] - cdfmin) / denom) * 255.0);
        hist_eq[i] = (unsigned int)round(val);
    }
    free(cdf);
    return hist_eq;
}

void bmp8_equalize(t_bmp8 *img, unsigned int *hist_eq) {
    if (!img || !img->data || !hist_eq) {
        fprintf(stderr, "bmp8_equalize: pointeur invalide\n");
        return;
    }
    for (unsigned int i = 0; i < img->dataSize; i++) {
        img->data[i] = (unsigned char)hist_eq[ img->data[i] ];
    }
}


// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////// //

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../bmp_24/bmp24.h"      // t_bmp24 et t_pixel

void bmp24_equalize(t_bmp24 *img) {
    if (!img || !img->data) {
        fprintf(stderr, "bmp24_equalize: img invalide\n");
        return;
    }

    int w = img->width;
    int h = img->height;
    int N = w * h;

    // 1) Histogramme de la luminance Y
    unsigned int hist[256] = {0};
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            t_pixel p = img->data[y][x];
            int Y = (int)round(0.299 * p.red + 0.587 * p.green + 0.114 * p.blue);
            hist[Y]++;
        }
    }

    // 2) CDF cumulée
    unsigned int cdf[256];
    unsigned int cum = 0;
    for (int i = 0; i < 256; i++) {
        cum    += hist[i];
        cdf[i] = cum;
    }

    // 3) cdfmin
    unsigned int cdfmin = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf[i] != 0) {
            cdfmin = cdf[i];
            break;
        }
    }

    // 4) Table de mapping Y → Y'
    unsigned char mapY[256];
    double denom = (double)(N - cdfmin);
    for (int i = 0; i < 256; i++) {
        double v;
        if (denom <= 0.0) {
            // Image uniforme ou un seul niveau
            v = ((double)cdf[i] / N) * 255.0;
        } else {
            v = ((double)(cdf[i] - cdfmin) / denom) * 255.0;
        }
        mapY[i] = (unsigned char)round(v);
    }

    // 5) Ré-application sur chaque pixel
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            t_pixel p = img->data[y][x];
            double R = p.red, G = p.green, B = p.blue;
            double Y  = 0.299 * R + 0.587 * G + 0.114 * B;
            double U  = (B - Y) * 0.565;
            double V  = (R - Y) * 0.713;

            int Y1 = mapY[(int)round(Y)];
            int R1 = (int)round(Y1 + 1.403 * V);
            int G1 = (int)round(Y1 - 0.344 * U - 0.714 * V);
            int B1 = (int)round(Y1 + 1.773 * U);

            // clamp manuel
            if      (R1 <   0) R1 =   0; else if (R1 > 255) R1 = 255;
            if      (G1 <   0) G1 =   0; else if (G1 > 255) G1 = 255;
            if      (B1 <   0) B1 =   0; else if (B1 > 255) B1 = 255;

            img->data[y][x].red   = (unsigned char)R1;
            img->data[y][x].green = (unsigned char)G1;
            img->data[y][x].blue  = (unsigned char)B1;
        }
    }
}
