#ifndef BMP_8_H
#define BMP_8_H

// Definition des structures

typedef struct {
    unsigned char header[54];
    unsigned char colorTable[1024];
    unsigned char* data;

    unsigned int width;
    unsigned int height;
    unsigned int colorDepth;
    unsigned int dataSize;
}t_bmp8;

// Appel des fonctions pour le fichier bmp8

t_bmp8* bmp8_loadImage(const char* filename);
void t_bmp8_saveImage(const char* filename, t_bmp8* img);
void bmp8_free(t_bmp8* img);
void bmp8_printInfo(t_bmp8* img);

// Fonctions pour traitement d'image

void bmp8_negative (t_bmp8* img);
void bmp8_brightness (t_bmp8* img , int value );
void bmp8_threshold (t_bmp8* img , int threshold);

// Fonction de filtre

void bmp8_applyFilter(t_bmp8 * img, float ** kernel, int kernelSize);

#endif //BMP_8_H
