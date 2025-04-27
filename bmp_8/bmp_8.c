#include "bmp_8.h"
#include<stdio.h>
#include<stdlib.h>
#include <string.h>

t_bmp8 * bmp8_loadImage(const char * filename) {
    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return NULL;
    }

    // Allocation de la structure image
    t_bmp8 *img = malloc(sizeof(t_bmp8));
    if (img == NULL) {
        perror("Erreur d'allocation mémoire pour l'image");
        fclose(f);
        return NULL;
    }

    // Lecture de l'en-tête (54 octets)
    fread(img->header, sizeof(unsigned char), 54, f);

    // Lecture de la table de couleur (1024 octets pour image 8 bits)
    fread(img->colorTable, sizeof(unsigned char), 1024, f);

    // Extraction des informations de l’en-tête
    img->width       = *(unsigned int*)&img->header[18];
    img->height      = *(unsigned int*)&img->header[22];
    img->colorDepth  = *(unsigned int*)&img->header[28];
    img->dataSize    = *(unsigned int*)&img->header[34];

    // Vérification que l'image est bien en niveaux de gris (8 bits)
    if (img->colorDepth != 8) {
        fprintf(stderr, "Erreur : profondeur de couleur non supportée (%d bits).\n", img->colorDepth);
        free(img);
        fclose(f);
        return NULL;
    }

    // Allocation de la mémoire pour les données de l’image
    img->data = malloc(img->dataSize);
    if (img->data == NULL) {
        perror("Erreur d'allocation mémoire pour les données");
        free(img);
        fclose(f);
        return NULL;
    }

    // Lecture des données de l’image
    fread(img->data, sizeof(unsigned char), img->dataSize, f);

    fclose(f);
    return img;
}

void bmp8_saveImage(const char *filename, t_bmp8 *img) {
    FILE *f = fopen(filename, "wb");
    if (f == NULL) {
        perror("Erreur lors de l'ouverture du fichier en écriture");
        return;
    }

    // Écrire l'en-tête (54 octets)
    fwrite(img->header, sizeof(unsigned char), 54, f);

    // Écrire la table de couleur (1024 octets)
    fwrite(img->colorTable, sizeof(unsigned char), 1024, f);

    // Écrire les données de l'image
    fwrite(img->data, sizeof(unsigned char), img->dataSize, f);

    fclose(f);
}

void bmp8_free(t_bmp8 *img) {
    if (img != NULL) {
        if (img->data != NULL) {
            free(img->data);
        }
        free(img);
    }
}


void bmp8_printInfo(t_bmp8 *img) {
    if (img == NULL) {
        printf("Aucune image chargée.\n");
        return;
    }

    printf("Image Info:\n");
    printf("Width: %u\n", img->width);
    printf("Height: %u\n", img->height);
    printf("Color Depth: %u\n", img->colorDepth);
    printf("Data Size: %u bytes\n", img->dataSize);
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////: //

void bmp8_negative (t_bmp8* img) {
    for ( int i = 0 ; i<(img->height * img->width) ; i++ ) {
        img->data[i] = 255 - img->data[i];
    }
}

void bmp8_brightness (t_bmp8* img , int value ) {
    int tmp = 0;
    for ( int i = 0 ; i<(img->height * img->width) ; i++ ) {
        tmp = img->data[i] + value;
        if ( tmp > 255 ) {
            img->data[i] = 255;
        } else if ( tmp < 0) {
            img->data[i] = 0;
        } else {
            img->data[i] = tmp;
        }
    }
}

void bmp8_threshold (t_bmp8* img , int threshold) {
    for ( int i = 0 ; i<(img->height * img->width) ; i++ ) {
        if ( img->data[i] >= threshold ) {
            img->data[i] = 255;
        }else {
            img->data[i] = 0;
        }
    }
}

void bmp8_applyFilter(t_bmp8 * img, float ** kernel, int kernelSize) {
    float tmp = 0 ;
    int height = img->height;
    int width = img->width;
    unsigned char* copy = ( unsigned char* ) malloc ( height * width );

    memcpy( copy , img->data , height * width );

    for ( int i = 1 ; i < ( height - 1 ) ; i++ ) {
        for ( int j = 1 ; j < ( width - 1 ) ; j++ ) {
            for ( int k = -( kernelSize / 2 ) ; k <= kernelSize/2 ; k++ ) {
                for ( int l = -( kernelSize / 2 ) ; l <= kernelSize/2 ; l++ ) {
                    tmp += copy [ ( i + k ) * ( width) + j + l ] * kernel[ kernelSize / 2 - k] [ kernelSize / 2 - l ];
                }
            }
            if ( tmp < 0 ) tmp = 0;
            if ( tmp > 255 ) tmp = 255;
            img->data[ i * width + j ] = ( unsigned char )tmp ;
            tmp = 0;
        }
    }
    free(copy);
}

