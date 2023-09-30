#ifndef _MCU_H_
#define _MCU_H_
#include <stdlib.h>
#include <stdint.h>

#define BLOC_SIZE 8 
/*
Effectue la reorganisation et le la quantification sur "matrice".
*/
extern int32_t *reorganization_zigzag(int32_t *matrice, uint8_t *qtable);

/*
Effectue uniquement quantification sur "matrice".
*/
extern void quantification(int32_t* matrice, uint8_t *table);

/*
Affiche la matrice représentée par vector.
*/
extern void print_vector(int32_t *vector);

/*
Sépare les 3 composantes RGB concaténées et traduit directement le triplet (R,G,B) en triplet (Y, Cb, Cr).  
*/
extern void rgb_to_YcbCr(int32_t* img, int32_t* R, int32_t* G, int32_t* B, uint32_t total_length);

/*
Retourne le sous vecteur de vector[start:stop+1]
*/
extern int32_t* tronc_vector(int32_t* vector, uint16_t start, uint16_t stop);

#endif /* _MCU_H_ */
