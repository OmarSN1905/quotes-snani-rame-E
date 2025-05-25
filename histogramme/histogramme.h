//
// Created by snani on 24/05/2025.
//

#ifndef HISTOGRAMME_H
#define HISTOGRAMME_H

#include "../bmp_8/bmp_8.h"
#include "../bmp_24/bmp_24.h"


unsigned int * bmp8_computeHistogram(t_bmp8 * img);
unsigned int * bmp8_computeCDF(unsigned int * hist);
void bmp8_equalize(t_bmp8 * img, unsigned int * hist_eq);

void bmp24_equalize(t_bmp24* img);


#endif //HISTOGRAMME_H
