#include "bmp_24.h"
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>

t_pixel** bmp24_allocateDataPixels (int width, int height) {
    t_pixel** mat_pixel = (t_pixel**) malloc ( height * sizeof( t_pixel* ));
    if ( mat_pixel == NULL ) {
        printf("Allocation failed !\n");
        return NULL;
    }
    for ( int i = 0 ; i < height ; i++ ) {
        mat_pixel[i] = ( t_pixel* ) malloc ( width * sizeof( t_pixel ));
        if ( mat_pixel[i] == NULL ) {
            printf("Allocation failed !\n");
            for ( int j = 0 ; j < i ; j++) {
                free ( mat_pixel[j]);
            }
            free ( mat_pixel );
            return NULL;
        }
    }
    printf("Allocation succeed !\n");
    return mat_pixel;
}




void bmp24_freeDataPixels (t_pixel** pixels, int height);
t_bmp24 * bmp24_allocate (int width, int height, int colorDepth);
void bmp24_free (t_bmp24 * img);