#ifndef BMP_8_H
#define BMP_8_H
#include <stdio.h>

// Structure représentant une image BMP en 8 bits (niveaux de gris)
typedef struct {
    unsigned char header[54];      // En-tête BMP standard (54 octets)
    unsigned char colorTable[1024];// Table des couleurs (256 couleurs × 4 octets)
    unsigned char*    data;        // Pointeur vers les données pixel (grayscale)
    unsigned int      width;       // Largeur de l'image en pixels
    unsigned int      height;      // Hauteur de l'image en pixels
    unsigned int      colorDepth;  // Profondeur de couleur (doit être 8 pour ce format)
    unsigned int      dataSize;    // Taille totale des données pixel en octets
} t_bmp8;

// ======================
//      Lecture/écriture
// ======================

// Charge une image BMP 8 bits depuis un fichier
t_bmp8* bmp8_loadImage(const char* filename);

// Sauvegarde une image BMP 8 bits dans un fichier
void bmp8_saveImage(const char* filename, t_bmp8* img);

// Libère toute la mémoire associée à une image BMP 8 bits
void bmp8_free(t_bmp8* img);

// Affiche les informations principales d’une image BMP (dimensions, taille, etc.)
void bmp8_printInfo(t_bmp8* img);

// ======================
//     Traitements de base
// ======================

// Applique un négatif (inversion des niveaux de gris)
void bmp8_negative(t_bmp8* img);

// Ajuste la luminosité de l’image (positif = plus clair, négatif = plus sombre)
void bmp8_brightness(t_bmp8* img, int value);

// Applique un seuillage : pixels >= seuil → blanc ; sinon → noir
void bmp8_threshold(t_bmp8* img, int threshold);

// ======================
//    Filtre par convolution
// ======================

// Applique un filtre convolutif (noyau carré) à l’image
void bmp8_applyFilter(t_bmp8* img, float** kernel, int kernelSize);

#endif //BMP_8_H
