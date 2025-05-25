#ifndef HISTOGRAMME_H
#define HISTOGRAMME_H

#include "../bmp_8/bmp_8.h"
#include "../bmp_24/bmp_24.h"

// === BMP 8 bits ===

/* Calcule l'histogramme de l'image (256 niveaux de gris) */
unsigned int * bmp8_computeHistogram(t_bmp8 * img);

/* Calcule la CDF normalisée à partir de l'histogramme */
unsigned int * bmp8_computeCDF(unsigned int * hist);

/* Applique l'égalisation via la table de remapping fournie */
void bmp8_equalize(t_bmp8 * img, unsigned int * hist_eq);


// === BMP 24 bits ===

/* Égalisation sur la luminance (Y), avec conversion RGB ↔ YUV */
void bmp24_equalize(t_bmp24* img);


#endif //HISTOGRAMME_H
