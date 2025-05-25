#include "bmp_8.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

t_bmp8* bmp8_loadImage(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        perror("Erreur lors de l'ouverture du fichier");
        return NULL;
    }

    t_bmp8* img = malloc(sizeof(t_bmp8));
    if (!img) {
        perror("Erreur d'allocation mémoire pour t_bmp8");
        fclose(f);
        return NULL;
    }

    // Lecture de l'en-tête et de la table de couleur
    if (fread(img->header,     1, 54,   f) != 54 ||
        fread(img->colorTable, 1, 1024, f) != 1024) {
        fprintf(stderr, "Erreur de lecture de l'en-tête\n");
        free(img);
        fclose(f);
        return NULL;
    }

    // Extraction des métadonnées
    img->width      = *(unsigned int*)&img->header[18];
    img->height     = *(unsigned int*)&img->header[22];
    img->colorDepth = *(unsigned int*)&img->header[28];
    img->dataSize   = *(unsigned int*)&img->header[34];
    if (img->colorDepth != 8) {
        fprintf(stderr, "Erreur : profondeur non supportée (%u bits)\n", img->colorDepth);
        free(img);
        fclose(f);
        return NULL;
    }
    if (img->dataSize == 0)
        img->dataSize = img->width * img->height;

    // Lecture des pixels
    img->data = malloc(img->dataSize);
    if (!img->data) {
        perror("Erreur d'allocation pour les données");
        free(img);
        fclose(f);
        return NULL;
    }
    if (fread(img->data, 1, img->dataSize, f) != img->dataSize) {
        fprintf(stderr, "Erreur de lecture des données pixel\n");
        free(img->data);
        free(img);
        fclose(f);
        return NULL;
    }

    fclose(f);
    return img;
}

void bmp8_saveImage(const char* filename, t_bmp8* img) {
    FILE* f = fopen(filename, "wb");
    if (!f) {
        perror("Erreur à l'ouverture en écriture");
        return;
    }
    fwrite(img->header,     1, 54,    f);
    fwrite(img->colorTable, 1, 1024,  f);
    fwrite(img->data,       1, img->dataSize, f);
    fclose(f);
}

void bmp8_free(t_bmp8* img) {
    if (!img) return;
    free(img->data);
    free(img);
}

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

void bmp8_negative(t_bmp8* img) {
    unsigned int n = img->width * img->height;
    for (unsigned int i = 0; i < n; i++)
        img->data[i] = 255 - img->data[i];
}

void bmp8_brightness(t_bmp8* img, int value) {
    unsigned int n = img->width * img->height;
    for (unsigned int i = 0; i < n; i++) {
        int v = img->data[i] + value;
        if (v <  0)   v = 0;
        if (v > 255) v = 255;
        img->data[i] = (unsigned char)v;
    }
}

void bmp8_threshold(t_bmp8* img, int threshold) {
    unsigned int n = img->width * img->height;
    for (unsigned int i = 0; i < n; i++)
        img->data[i] = (img->data[i] >= threshold) ? 255 : 0;
}

void bmp8_applyFilter(t_bmp8* img, float** kernel, int kernelSize) {
    int  w = img->width, h = img->height;
    int  r = kernelSize / 2;
    unsigned char* src = malloc(img->dataSize);
    if (!src) return;
    memcpy(src, img->data, img->dataSize);

    // Convolution (sans modifier les bords)
    for (int y = r; y < h - r; y++) {
        for (int x = r; x < w - r; x++) {
            float sum = 0.0f;
            for (int ky = -r; ky <= r; ky++) {
                for (int kx = -r; kx <= r; kx++) {
                    unsigned char p = src[(y + ky) * w + (x + kx)];
                    sum += p * kernel[ky + r][kx + r];
                }
            }
            if (sum < 0)   sum = 0;
            if (sum > 255) sum = 255;
            img->data[y * w + x] = (unsigned char)sum;
        }
    }

    free(src);
}
