#ifndef _RLE_H_
#define _RLE_H_

#include <stdlib.h>
#include <stdio.h>
#include <mcu.h>
#include <rle.h>
#include <my_bitstream.h> 
#include <stdint.h>
#include <stdbool.h>
#include <me_jpeg_writer.h>
#include <huffman.h>
/*
        Fait la différence terme à terme entre les termes successifs du vecteur.
*/
extern void difference(int32_t * matrix);

/*
        Renvoie l'indice de l'élèment value 
*/
extern int8_t in_array(int32_t * matrix, const uint8_t len, int32_t value);

/*
        Donne l'indexe de l'entrée val.
*/
extern uint16_t get_index(int32_t val);

extern uint16_t get_max(uint8_t indice_fort);

/*
        Donne la magnétude de la valeur passée en paramètre.
*/
extern uint8_t magnetude(int32_t val);

/*
        Encode les coefficients AC de la matrice "matrix". On donne la table de huffman associée ainsi 
        que le bitsream du fichier jpeg. 
*/
extern void encode_dc(struct my_bitstream * bs, int32_t matrix, struct huff_table* ht);

/*
        Retourne l'indice du premier élement non à partir de start.
        le maximum est 17 (16 +1 car on envoie l'indice d'après d'après.)
*/
extern uint8_t longest_seq0(int32_t * matrix, uint8_t start);

/*
        Encode le coefficient DC de la matrice. "matrix" est la valeur de ce DC. 
        La structure my_bitstream est celle de la structure jpeg.
*/
extern void encode_ac(struct my_bitstream * bs, int32_t *matrix, struct huff_table* ht);


#endif /* RLE_H */
