#ifndef DOWNSAMPLING_H
#define DOWNSAMPLING_H

#include <stdint.h>

/*
    Effectue un sous-échantillonnage horizontal sur la matrice
    d'entrée.
*/
extern int32_t *horizontal_downsampling(int32_t *matrix, uint16_t size, uint8_t h1, uint8_t h2);
/*
    Effectue un sous-échantillonnage vertical sur la matrice
    d'entrée.
*/
extern int32_t *vertical_downsampling(int32_t *matrix, uint16_t size, uint8_t h1, uint8_t v1, uint8_t v2);
/*
    Effectue un sous-échantillonnage vertical et horizontal sur
    la matrice d'entrée.
*/
int32_t *bidimensional_downsampling(int32_t *matrix, uint16_t size, uint8_t h1, uint8_t v1, uint8_t h2, uint8_t v2);
/*
    Fonction effectuant le sous-échantillonnage suivant les facteurs
    de subsampling en entrée.
*/
extern int32_t *downsampling(int32_t *matrix, uint16_t size, uint8_t h1, uint8_t v1, uint8_t h2, uint8_t v2);
/*
    Réordonne une matrice sous-échantillonnée afin de lire bloc par
    bloc.
*/
extern int32_t *reorder(int32_t *matrix, uint16_t size, uint8_t v);


#endif /* DOWNSAMPLING_H */
