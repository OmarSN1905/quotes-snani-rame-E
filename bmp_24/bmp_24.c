#include "bmp_24.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//bibliotheque

// les fonctions

t_pixel ** bmp24_allocateDataPixels (int width, int height) {
    t_pixel ** pixels;
    int i, j;
    pixels = (t_pixel **) malloc (sizeof (t_pixel *) * height);
    if (pixels == NULL) {
        printf ("Erreur, allocation echoué ");
        return NULL;
    }
    for (i = 0; i < height; i++) {
        pixels[i] = (t_pixel *) malloc (width * sizeof (t_pixel));
        if (pixels[i] == NULL) {
            printf("Erreur, alloction echoué");
            for (j = 0; j < i; j++) {
                free(pixels[j]);
            }
            free(pixels);
            return NULL;
        }
    }
    printf("Allocation reussie ");
    return pixels;
}


void bmp24_freeDataPixels (t_pixel ** pixels, int height){
    int i;
    if (pixels == NULL) {
        return;
    }
    for (i = 0; i < height; i++) {
        free(pixels[i]);
    }
    free(pixels);
}

t_bmp24 * bmp24_allocate (int width, int height, int colorDepth) {
    t_bmp24 * img;
    img = (t_bmp24 *) malloc (sizeof (t_bmp24));
    if (img == NULL) {
        printf("allocation echoué");
        return NULL;
    }

    img->data=bmp24_allocateDataPixels (width, height);
    if (img->data == NULL) {
        free(img);
        printf("Erreur, echec de l'alocation ");
        return NULL;
    }
    return img;
}

void bmp24_free (t_bmp24 * img) {
    int i;
    if (img == NULL) {
        return;
    }
    for (i = 0; i < img->height; i++) {
        free(img->data[i]);
    }
    free(img->data);
    free(img);
}

void  file_rawRead (uint32_t position, void * buffer, uint32_t size, size_t n, FILE * file) {
    fseek (file, position, SEEK_SET);
    fread (buffer, size, n, file);
}

void  file_rawWrite ( uint32_t position , void * buffer, uint32_t size, size_t n, FILE * file ) {
    fseek (file, position, SEEK_SET);
    fwrite (buffer, size, n, file);
}

int bmp24_readPixelData(FILE *file, t_bmp24 *img) {
    int i, j;
    int rowSize = ((img->width * 3 + 3) / 4) * 4; // taille ligne avec padding
    uint8_t *row = malloc(rowSize);
    if (!row) return 0;

    // Les lignes sont stockées du bas vers le haut
    for (i = img->height - 1; i >= 0; i--) {
        if (fread(row, 1, rowSize, file) != rowSize) {
            free(row);
            return 0;
        }
        for (j = 0; j < img->width; j++) {
            img->data[i][j].blue  = row[j*3 + 0]; // B
            img->data[i][j].green = row[j*3 + 1]; // G
            img->data[i][j].red   = row[j*3 + 2]; // R
        }
    }
    free(row);
    return 1;
}

void bmp24_writePixelValue(t_bmp24 *img, int x, int y, FILE *file) {
    uint8_t rgb[3] = {
        img->data[y][x].red,
        img->data[y][x].green,
        img->data[y][x].blue
    };
    fwrite(rgb, 1, 3, file);
}

void bmp24_readPixelValue(t_bmp24 *img, int x, int y, FILE *file) {
    uint8_t rgb[3];
    if (fread(rgb, 1, 3, file) == 3) {
        img->data[y][x].red = rgb[0];
        img->data[y][x].green = rgb[1];
        img->data[y][x].blue = rgb[2];
    }
}

int bmp24_writePixelData(FILE *file, t_bmp24 *img) {
    int i, j;
    int rowSize = ((img->width * 3 + 3) / 4) * 4;
    uint8_t *row = calloc(rowSize, 1); // padding à 0
    if (!row) return 0;

    for (i = img->height - 1; i >= 0; i--) {
        for (j = 0; j < img->width; j++) {
            row[j*3 + 0] = img->data[i][j].blue;
            row[j*3 + 1] = img->data[i][j].green;
            row[j*3 + 2] = img->data[i][j].red;
        }
        fwrite(row, 1, rowSize, file);
    }
    free(row);
    return 1;
}

t_bmp24 * bmp24_loadImage (const char * filename) {
    FILE *file;
    file = fopen(filename, "rb");
    if (file == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s\n", filename);
        return NULL;
    }

    // Lire l'en-tête BMP
    t_bmp_header header;
    t_bmp_info info;

    // Lire le type de fichier
    file_rawRead(BITMAP_MAGIC, &header.type, sizeof(uint16_t), 1, file);
    if (header.type != BMP_TYPE) {
        printf("Erreur : Le fichier n'est pas un BMP valide\n");
        fclose(file);
        return NULL;
    }

    // Lire le reste de l'en-tête
    file_rawRead(BITMAP_SIZE, &header.size, sizeof(uint32_t), 1, file);
    file_rawRead(BITMAP_OFFSET, &header.offset, sizeof(uint32_t), 1, file);

    // Lire l'en-tête d'information
    file_rawRead(BITMAP_WIDTH, &info.width, sizeof(int32_t), 1, file);
    file_rawRead(BITMAP_HEIGHT, &info.height, sizeof(int32_t), 1, file);
    file_rawRead(BITMAP_DEPTH, &info.bits, sizeof(uint16_t), 1, file);

    // Vérifier la profondeur de couleur
    if (info.bits != DEFAULT_DEPTH) {
        printf("Erreur : Seules les images 24 bits sont supportées\n");
        fclose(file);
        return NULL;
    }

    // Allouer la structure d'image
    t_bmp24* image = bmp24_allocate(info.width, info.height, info.bits);
    if (!image) {
        printf("Erreur : Impossible d'allouer la mémoire pour l'image\n");
        fclose(file);
        return NULL;
    }

    // Copier les en-têtes
    image->header = header;
    image->header_info = info;
    image->width = info.width;
    image->height = info.height;
    image->colorDepth = info.bits;

    // Lire les données des pixels
    if (!bmp24_readPixelData(file, image)) {
        printf("Erreur : Impossible de lire les données des pixels\n");
        fclose(file);
        bmp24_free(image);
        return NULL;
    }

    fclose(file);
    return image;
}

void bmp24_saveImage(t_bmp24 *img, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Erreur : impossible d'ouvrir le fichier %s\n", filename);
        return;
    }

    int rowSize = ((img->width * 3 + 3) / 4) * 4;
    int pixelDataSize = rowSize * img->height;

    // Mettre à jour les en-têtes
    img->header.type = BMP_TYPE;
    img->header.size = 14 + 40 + pixelDataSize; // 14 pour header BMP, 40 pour info header
    img->header.reserved1 = 0;
    img->header.reserved2 = 0;
    img->header.offset = 14 + 40;

    img->header_info.size = 40;
    img->header_info.width = img->width;
    img->header_info.height = img->height;
    img->header_info.planes = 1;
    img->header_info.bits = 24;
    img->header_info.compression = 0;
    img->header_info.imagesize = pixelDataSize;
    img->header_info.xresolution = 2835;
    img->header_info.yresolution = 2835;
    img->header_info.ncolors = 0;
    img->header_info.importantcolors = 0;

    // Écriture de l'en-tête BMP (14 octets, champ par champ pour éviter padding)
    fwrite(&img->header.type, sizeof(uint16_t), 1, file);
    fwrite(&img->header.size, sizeof(uint32_t), 1, file);
    fwrite(&img->header.reserved1, sizeof(uint16_t), 1, file);
    fwrite(&img->header.reserved2, sizeof(uint16_t), 1, file);
    fwrite(&img->header.offset, sizeof(uint32_t), 1, file);

    // Écriture de l'info header (40 octets, struct entière car pas de padding dans t_bmp_info)
    fwrite(&img->header_info, sizeof(t_bmp_info), 1, file);

    // Écriture des pixels (avec padding automatique)
    uint8_t *row = calloc(rowSize, 1); // ligne temporaire avec padding 0
    int i, j;
    for (i = img->height - 1; i >= 0; i--) {
        for (j = 0; j < img->width; j++) {
            row[j*3 + 0] = img->data[i][j].blue;
            row[j*3 + 1] = img->data[i][j].green;
            row[j*3 + 2] = img->data[i][j].red;
        }
        fwrite(row, 1, rowSize, file);
    }
    free(row);

    fclose(file);
    printf("✅ Image BMP sauvegardée dans '%s' avec succès.\n", filename);
}



void bmp24_negative (t_bmp24 * img) {
    int i, j;
    for (i = 0; i < img->height; i++) {
        for (j = 0; j < img->width; j++) {
            img->data[i][j].red = 255 - img->data[i][j].red;
            img->data[i][j].green = 255 - img->data[i][j].green;
            img->data[i][j].blue = 255 - img->data[i][j].blue;
        }
    }
}

void bmp24_grayscale ( t_bmp24 * img) {
    int i, j;
    int red = 0, green = 0, blue = 0;
    for (i = 0; i < img->height; i++) {
        for (j = 0; j < img->width; j++) {
            red = img->data[i][j].red;
            green = img->data[i][j].green;
            blue = img->data[i][j].blue;

            img->data[i][j].red = (red + green + blue ) / 3;
            img->data[i][j].green = (red + green + blue) / 3;
            img->data[i][j].blue = (red + green + blue) / 3;
        }
    }
}

void bmp24_brightness (t_bmp24 * img, int value) {
    int i, j;
    for (i = 0; i < img->height; i++) {
        for (j = 0; j < img->width; j++) {
            img->data[i][j].red = img->data[i][j].red + value;
            if (img->data[i][j].red > 255) {
                img->data[i][j].red = 255;
            }
            img->data[i][j].green = img->data[i][j].green + value;
            if (img->data[i][j].green > 255) {
                img->data[i][j].green = 255;
            }
            img->data[i][j].blue = img->data[i][j].blue + value;
            if (img->data[i][j].blue > 255) {
                img->data[i][j].blue = 255;
            }
        }
    }
}
t_pixel bmp24_convolution (t_bmp24 * img, int x, int y, float ** kernel, int kernelSize) {
    int i, j;
    int half = kernelSize / 2;
    float r = 0, g = 0, b = 0;
    for (i = -half; i <= half; i++) {
        for (j = -half; j <= half; j++) {
            int line = x + i;
            int col = y + j;
            if (line >= 0 && line < img->height && col >= 0 && col < img->width) {
                float coeff = kernel[i + half][j + half];
                r += img->data[line][col].red * coeff;
                g += img->data[line][col].green * coeff;
                b += img->data[line][col].blue * coeff;
            }
        }
    }
    t_pixel result;

    if (r < 0)
        result.red = 0;
    else if (r > 255)
        result.red = 255;
    else
        result.red = r;

    if (g < 0)
        result.green = 0;
    else if (g > 255)
        result.green = 255;
    else
        result.green = g;

    if (b < 0)
        result.blue = 0;
    else if (b > 255)
        result.blue = 255;
    else
        result.blue = b;
    return result;
}

// Fonction utilitaire pour créer un noyau de convolution
float** create_kernel(int size) {
    float** kernel = (float**)malloc(size * sizeof(float*));
    int i;
    for (i = 0; i < size; i++) {
        kernel[i] = (float*)malloc(size * sizeof(float));
    }
    return kernel;
}

// Fonction utilitaire pour libérer un noyau de convolution
void free_kernel(float** kernel, int size) {
    int i;
    for (i = 0; i < size; i++) {
        free(kernel[i]);
    }
    free(kernel);
}

void bmp24_boxBlur(t_bmp24 *img) {
    int i, j;
    int kernelSize = 3;
    float** kernel = create_kernel(kernelSize);

    // Initialisation du noyau de flou moyen
    for (i = 0; i < kernelSize; i++) {
        for (j = 0; j < kernelSize; j++) {
            kernel[i][j] = 1.0f / (kernelSize * kernelSize);
        }
    }

    // Création d'une copie temporaire de l'image
    t_pixel** temp = bmp24_allocateDataPixels(img->width, img->height);
    for (i = 0; i < img->height; i++) {
        for (j = 0; j < img->width; j++) {
            temp[i][j] = bmp24_convolution(img, i, j, kernel, kernelSize);
        }
    }

    // Copie du résultat dans l'image originale
    for (i = 0; i < img->height; i++) {
        for (j = 0; j < img->width; j++) {
            img->data[i][j] = temp[i][j];
        }
    }

    bmp24_freeDataPixels(temp, img->height);
    free_kernel(kernel, kernelSize);
}

void bmp24_gaussianBlur(t_bmp24 *img) {
    int i, j;
    int kernelSize = 3;
    float** kernel = create_kernel(kernelSize);

    // Initialisation du noyau gaussien
    float gaussian[3][3] = {
        {1.0f/16, 2.0f/16, 1.0f/16},
        {2.0f/16, 4.0f/16, 2.0f/16},
        {1.0f/16, 2.0f/16, 1.0f/16}
    };

    for (i = 0; i < kernelSize; i++) {
        for (j = 0; j < kernelSize; j++) {
            kernel[i][j] = gaussian[i][j];
        }
    }

    t_pixel** temp = bmp24_allocateDataPixels(img->width, img->height);
    for (i = 0; i < img->height; i++) {
        for (j = 0; j < img->width; j++) {
            temp[i][j] = bmp24_convolution(img, i, j, kernel, kernelSize);
        }
    }

    for (i = 0; i < img->height; i++) {
        for (j = 0; j < img->width; j++) {
            img->data[i][j] = temp[i][j];
        }
    }

    bmp24_freeDataPixels(temp, img->height);
    free_kernel(kernel, kernelSize);
}

void bmp24_outline(t_bmp24 *img) {
    int i, j;
    int kernelSize = 3;
    float** kernel = create_kernel(kernelSize);

    // Initialisation du noyau de détection de contours
    float outline[3][3] = {
        {-1, -1, -1},
        {-1,  8, -1},
        {-1, -1, -1}
    };

    for (i = 0; i < kernelSize; i++) {
        for (j = 0; j < kernelSize; j++) {
            kernel[i][j] = outline[i][j];
        }
    }

    t_pixel** temp = bmp24_allocateDataPixels(img->width, img->height);
    for (i = 0; i < img->height; i++) {
        for (j = 0; j < img->width; j++) {
            temp[i][j] = bmp24_convolution(img, i, j, kernel, kernelSize);
        }
    }

    for (i = 0; i < img->height; i++) {
        for (j = 0; j < img->width; j++) {
            img->data[i][j] = temp[i][j];
        }
    }

    bmp24_freeDataPixels(temp, img->height);
    free_kernel(kernel, kernelSize);
}

void bmp24_emboss(t_bmp24 *img) {
    int i, j;
    int kernelSize = 3;
    float** kernel = create_kernel(kernelSize);

    // Initialisation du noyau de relief
    float emboss[3][3] = {
        {-2, -1,  0},
        {-1,  1,  1},
        { 0,  1,  2}
    };

    for (i = 0; i < kernelSize; i++) {
        for (j = 0; j < kernelSize; j++) {
            kernel[i][j] = emboss[i][j];
        }
    }

    t_pixel** temp = bmp24_allocateDataPixels(img->width, img->height);
    for (i = 0; i < img->height; i++) {
        for (j = 0; j < img->width; j++) {
            temp[i][j] = bmp24_convolution(img, i, j, kernel, kernelSize);
        }
    }

    for (i = 0; i < img->height; i++) {
        for (j = 0; j < img->width; j++) {
            img->data[i][j] = temp[i][j];
        }
    }

    bmp24_freeDataPixels(temp, img->height);
    free_kernel(kernel, kernelSize);
}

void bmp24_sharpen(t_bmp24 *img) {
    int i, j;
    int kernelSize = 3;
    float** kernel = create_kernel(kernelSize);

    // Initialisation du noyau de netteté
    float sharpen[3][3] = {
        { 0, -1,  0},
        {-1,  5, -1},
        { 0, -1,  0}
    };

    for (i = 0; i < kernelSize; i++) {
        for (j = 0; j < kernelSize; j++) {
            kernel[i][j] = sharpen[i][j];
        }
    }

    t_pixel** temp = bmp24_allocateDataPixels(img->width, img->height);
    for (i = 0; i < img->height; i++) {
        for (j = 0; j < img->width; j++) {
            temp[i][j] = bmp24_convolution(img, i, j, kernel, kernelSize);
        }
    }

    for (i = 0; i < img->height; i++) {
        for (j = 0; j < img->width; j++) {
            img->data[i][j] = temp[i][j];
        }
    }

    bmp24_freeDataPixels(temp, img->height);
    free_kernel(kernel, kernelSize);
}