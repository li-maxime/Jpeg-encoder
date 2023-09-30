#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <downsampling.h>

/*
    Effectue un sous-échantillonnage horizontal sur la matrice
    d'entrée.
*/
int32_t *horizontal_downsampling(int32_t *matrix, uint16_t size, uint8_t h1,
                                 uint8_t h2)
{
    uint16_t resize_factor = h1/h2;
    uint16_t sampling_size = size/resize_factor;
    int32_t *new_matrix = (int32_t *) calloc(sampling_size, sizeof(int32_t));
    uint16_t k = 0;
    int32_t average = 0;
    for (uint16_t i = 0; i < size; i+=resize_factor){
        average = 0;
        //printf("\nAverage of :");
        for (uint16_t j = 0; j < resize_factor; j++){
            //printf(" %x ", matrix[i+j]);
            average += matrix[i + j];
        }

        average /= resize_factor;
        //printf("is : %x", average);
        new_matrix[k] = average;
        k++;
    }
    //printf("\n");
    //////printf("taille : %u \n", sampling_size);
    return new_matrix;

}
/*
    Effectue un sous-échantillonnage vertical sur la matrice
    d'entrée.
*/
int32_t *vertical_downsampling(int32_t *matrix, uint16_t size, uint8_t h1,
                                uint8_t v1, uint8_t v2)
{
    uint16_t resize_factor = v1/v2;
    uint16_t sampling_size = size/resize_factor;
    uint16_t line_size = 8*h1;
    uint16_t column_size = size/line_size;
    int32_t *new_matrix =  (int32_t *) calloc(sampling_size, sizeof(int32_t));
    uint16_t k = 0;
    int32_t average = 0;

    for (uint16_t m = 0; m < column_size/resize_factor; m++){
        for (uint16_t i = 0; i < line_size; i++){
            average = 0;
            //printf("\nAverage of :");
            for (uint16_t j = 0; j < resize_factor; j++){
                //printf(" %x ", matrix[i + line_size*j + line_size*resize_factor*m]);
                average += matrix[i + line_size*j + line_size*resize_factor*m];
            }
            average /= resize_factor;
            //printf("is : %x", average);
            new_matrix[k] = average;
            k++;
        }
    }
    //printf("\n");
    return new_matrix;

}
/*
    Effectue un sous-échantillonnage vertical et horizontal sur
    la matrice d'entrée.
*/
int32_t *bidimensional_downsampling(int32_t *matrix, uint16_t size, uint8_t h1, uint8_t v1, uint8_t h2, uint8_t v2)
{
    uint16_t resize_factor_v = v1/v2;
    uint16_t resize_factor_h = h1/h2;
    uint16_t sampling_size = size/(resize_factor_v*resize_factor_h);
    uint16_t line_size = 8*h1;
    uint16_t column_size = size/line_size;
    int32_t *new_matrix =  (int32_t *) calloc(sampling_size, sizeof(int32_t));
    uint16_t k = 0;
    int average = 0;


    for (uint16_t m = 0; m < column_size/resize_factor_v; m++){
        for (uint16_t i = 0; i < line_size; i+= resize_factor_h){
            average = 0;
            //printf("\nAverage of :");
            for (uint16_t j = 0; j < resize_factor_v; j++){
                for(uint16_t p = 0; p < resize_factor_h; p++){
                    //printf("%x ", matrix[i + p + line_size*j + line_size*resize_factor_v*m]);
                    average += matrix[i + p + line_size*j + line_size*resize_factor_v*m];
                }
            }
            average /= resize_factor_h*resize_factor_v;
            //printf(" is : %x", average);
            new_matrix[k] = average;
            k++;
        }
    }
    //printf("\n");
    //////printf("taille : %u \n", (column_size/resize_factor)*line_size);
    return new_matrix;
}


/*
    Fonction effectuant le sous-échantillonnage suivant les facteurs
    de subsampling en entrée.
*/
int32_t *downsampling(int32_t *matrix, uint16_t size, uint8_t h1, uint8_t v1, uint8_t h2, uint8_t v2)
{
    if (h1 == h2){
        if (v1 == v2){
            int32_t *new_matrix =  (int32_t *) calloc(size, sizeof(int32_t));
            memcpy(new_matrix, matrix, size*sizeof(int32_t));
            return new_matrix;
        }
        else{
            return vertical_downsampling(matrix, size, h1, v1, v2);
        }
    }
    else if (v1 == v2){
        return horizontal_downsampling(matrix, size, h1, h2);
    }
    else{
        return bidimensional_downsampling(matrix, size, h1, v1, h2, v2);
    }
}
/*
    Réordonne une matrice sous-échantillonnée afin de lire bloc par
    bloc.
*/
int32_t *reorder(int32_t *matrix, uint16_t size, uint8_t v)
{
    uint16_t start =0;
    int32_t *mcu = calloc(size, sizeof(int32_t));
    uint8_t lenght = 8*v;
    for (uint16_t i = 0; i < size; i++){
        if (i != 0 && i%64 == 0){
            start += 8;
            if (start % lenght == 0){
                start += 7*lenght;
            }
        }
        //////printf("%i = %i, %i, %i\n", i, start, ((i%64)/8)*lenght, i%8 );
        mcu[i] = matrix[start + ((i%64)/8)*lenght + (i % 8)];

    }
    return mcu;
}
