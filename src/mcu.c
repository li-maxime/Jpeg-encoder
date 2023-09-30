#include <stdlib.h>
#include <stdio.h>
#include <mcu.h>
#include <stdint.h>

/*
Vecteur contenant les indices pour le zigzag. 
zigzag_position[i] donne l'indice de la i-eme valeur à visiter dans le parcours zigzag. 
*/
static int zigzag_position[] = {
    0, 1, 5, 6, 14, 15, 27, 28,
    2, 4, 7, 13, 16, 26, 29, 42,
    3, 8, 12, 17, 25, 30, 41, 43,
    9, 11, 18, 24, 31, 40, 44, 53,
    10, 19, 23, 32, 39, 45, 52, 54,
    20, 22, 33, 38, 46, 51, 55, 60,
    21, 34, 37, 47, 50, 56, 59, 61,
    35, 36, 48, 49, 57, 58, 62, 63
};

/*
Effectue la reorganisation et le la quantification sur "matrice".
*/
int32_t *reorganization_zigzag(int32_t *matrice, uint8_t *qtable){
    int32_t *vector = malloc(64*sizeof(int));
    uint8_t index;
    for (int i = 0; i < 64; i++){
        index = zigzag_position[i];
        vector[index] = matrice[i]/qtable[index]; 
    }
    return vector;
}
/*
Effectue uniquement quantification sur "matrice".
*/
void quantification(int32_t* matrice, uint8_t *table){
    for (int i = 0; i < 64; i++){
        matrice[i] = matrice[i]/table[i]; 
    }
}
/*
Affiche la matrice représentée par vector.
*/
void print_vector(int32_t *vector){
    for (int i = 0; i<64; i++){
        if (i % 8 == 0){
            printf("\n");
        }
        printf("%i,", vector[i]);
    }
    printf("\n");
}

/*
Sépare les 3 composantes RGB concaténées et traduit directement le triplet (R,G,B) en triplet (Y, Cb, Cr).  
*/
void rgb_to_YcbCr(int32_t* img, int32_t* R, int32_t* G, int32_t* B, uint32_t total_length){
    int32_t r,g,b;
    for(uint64_t i = 0; i < total_length; i++){
        b = img[i]/65536;
        g = (img[i]-b*65536)/256;
        r = img[i] - g*256 - b*65536;

        R[i] = (int32_t) ((0.299*r + 0.587*g + 0.114*b) + 0.5);
        G[i] = (int32_t) ((-0.1687*r - 0.3313*g + 0.5*b + 128) + 0.5);
        B[i] = (int32_t) ((0.5*r - 0.4187*g - 0.0813*b + 128) + 0.5);
    }
 }

/*
Retourne le sous vecteur de vector[start:stop+1]
*/
int32_t* tronc_vector(int32_t* vector, uint16_t start, uint16_t stop){

    int32_t* troncate_v = (int32_t*) calloc(stop-start, sizeof(int32_t));
    for(uint16_t i = start; i < stop; i++){
        troncate_v[i-start] = vector[i];
    }
    return troncate_v;
}

/*
int main(void)
{
    int table[64];
    for (int i = 1; i<65; i++){
        table[i] = i;
    }
    print_vector(table);
    int *vector = reorganization_zigzag(table);
    print_vector(vector);
    quantification(vector, table);
    print_vector(vector);


    return EXIT_SUCCESS;
}
*/