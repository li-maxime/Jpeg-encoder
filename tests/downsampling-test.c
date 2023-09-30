#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/*
    Affiche la matrice reçue.
*/
 static void printf_table(int32_t *matrix, int size)
{
    for (int i = 0; i < size; i++){
        if(i%8 == 0){
            if (i%64 == 0){
                printf("\n");
            }
            printf("\n");
        }
        printf("%x ,", matrix[i]);
    }
    printf("\n");
}



/*
    Effectue un sous-échantillonnage horizontal sur la matrice
    d'entrée.
*/
static void horizontal_downsampling(int32_t *matrix, uint16_t size, uint8_t h1,
                                 uint8_t h2)
{
    uint16_t resize_factor = h1/h2;
    uint16_t k = 0;
    int32_t average = 0;
    for (uint16_t i = 0; i < size; i+=resize_factor){
        average = 0;
        printf("\nAverage of :");
        for (uint16_t j = 0; j < resize_factor; j++){
            printf(" %x ", matrix[i+j]);
            average += matrix[i + j];
        }

        average /= resize_factor;
        printf("is : %x", average);
        k++;
    }
    printf("\n");

}
/*
    Effectue un sous-échantillonnage vertical sur la matrice
    d'entrée.
*/
static void vertical_downsampling(int32_t *matrix, uint16_t size, uint8_t h1,
                                uint8_t v1, uint8_t v2)
{
    uint16_t resize_factor = v1/v2;
    uint16_t line_size = 8*h1;
    uint16_t column_size = size/line_size;
    uint16_t k = 0;
    int32_t average = 0;

    for (uint16_t m = 0; m < column_size/resize_factor; m++){
        for (uint16_t i = 0; i < line_size; i++){
            average = 0;
            printf("\nAverage of :");
            for (uint16_t j = 0; j < resize_factor; j++){
                printf(" %x ", matrix[i + line_size*j + line_size*resize_factor*m]);
                average += matrix[i + line_size*j + line_size*resize_factor*m];
            }
            average /= resize_factor;
            printf("is : %x", average);
            k++;
        }
    }
    printf("\n");

}
/*
    Effectue un sous-échantillonnage vertical et horizontal sur
    la matrice d'entrée.
*/
static void bidimensional_downsampling(int32_t *matrix, uint16_t size, uint8_t h1, uint8_t v1, uint8_t h2, uint8_t v2)
{
    uint16_t resize_factor_v = v1/v2;
    uint16_t resize_factor_h = h1/h2;
    uint16_t line_size = 8*h1;
    uint16_t column_size = size/line_size;
    uint16_t k = 0;
    int average = 0;


    for (uint16_t m = 0; m < column_size/resize_factor_v; m++){
        for (uint16_t i = 0; i < line_size; i+= resize_factor_h){
            average = 0;
            printf("\nAverage of :");
            for (uint16_t j = 0; j < resize_factor_v; j++){
                for(uint16_t p = 0; p < resize_factor_h; p++){
                    printf("%x ", matrix[i + p + line_size*j + line_size*resize_factor_v*m]);
                    average += matrix[i + p + line_size*j + line_size*resize_factor_v*m];
                }
            }
            average /= resize_factor_h*resize_factor_v;
            printf(" is : %x", average);
            k++;
        }
    }
    printf("\n");
}


/*
    Fonction effectuant le sous-échantillonnage suivant les facteurs
    de subsampling en entrée.
*/
static void downsampling(int32_t *matrix, uint16_t size, uint8_t h1, uint8_t v1, uint8_t h2, uint8_t v2)
{
    if (h1 == h2){
        if (v1 == v2){
            printf("Nothing's done.\n");
        }
        else{
            vertical_downsampling(matrix, size, h1, v1, v2);
        }
    }
    else if (v1 == v2){
        horizontal_downsampling(matrix, size, h1, h2);
    }
    else{
        bidimensional_downsampling(matrix, size, h1, v1, h2, v2);
    }
}


int main(void)
{

    int32_t matrix[64*4] = {0x7b, 0xbe, 0xa2, 0xa1, 0xa3, 0xa0, 0x9c, 0x99, 0x94, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
                             0X72, 0xac, 0x8d, 0x92, 0x99, 0x9e, 0xa2, 0xa5, 0xa7, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8, 0xa8,
                             0x4e, 0x69, 0x77, 0x7d, 0x79, 0x7e, 0x8e, 0x8d, 0x90, 0x96, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1, 0xa1,
                             0x57, 0x5d, 0x73, 0x77, 0x68, 0x77, 0xa4, 0x86, 0x79, 0x89, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f, 0x9f,
                             0x72, 0x88, 0x8f, 0x7f, 0x77, 0x8c, 0xb9, 0x93, 0x81, 0x93, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3, 0xa3,
                             0x72, 0x96, 0x91, 0x73, 0x7d, 0x82, 0x8f, 0x7c, 0x91, 0x9d, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
                             0x68, 0x77, 0x63, 0x55, 0x63, 0x67, 0x60, 0x55, 0x60, 0x6d, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 
                             0x68, 0x77, 0x63, 0x55, 0x63, 0x67, 0x60, 0x55, 0x60, 0x6d, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64, 
                            0x68, 0x77, 0x63, 0x55, 0x63, 0x67, 0x60, 0x55, 0x60, 0x6d, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
                            0x68, 0x77, 0x63, 0x55, 0x63, 0x67, 0x60, 0x55, 0x60, 0x6d, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
                            0x68, 0x77, 0x63, 0x55, 0x63, 0x67, 0x60, 0x55, 0x60, 0x6d, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
                            0x68, 0x77, 0x63, 0x55, 0x63, 0x67, 0x60, 0x55, 0x60, 0x6d, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
                            0x68, 0x77, 0x63, 0x55, 0x63, 0x67, 0x60, 0x55, 0x60, 0x6d, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
                            0x68, 0x77, 0x63, 0x55, 0x63, 0x67, 0x60, 0x55, 0x60, 0x6d, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
                            0x68, 0x77, 0x63, 0x55, 0x63, 0x67, 0x60, 0x55, 0x60, 0x6d, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64,
                            0x68, 0x77, 0x63, 0x55, 0x63, 0x67, 0x60, 0x55, 0x60, 0x6d, 0x64, 0x64, 0x64, 0x64, 0x64, 0x64};
    

    printf("\n\nOn commence par un sous-échantillonnage horizontal.\n");
    printf_table(matrix, 64*4);
    downsampling(matrix, 64*4, 2, 1, 1, 1);
    printf("\n\nOn continue avec un sous-échantillonnage vertical.\n");
    printf_table(matrix, 64*4);
    downsampling(matrix, 64*4, 1, 2, 1, 1);
    printf("\n\nEssayons un échantillonnage bidimensionnel.\n");
    printf_table(matrix, 64*4);
    downsampling(matrix, 64*4, 2, 2, 1, 1);

    return EXIT_SUCCESS;
}
