#include <stdlib.h>
#include <stdio.h>
#include <mcu.h>
#include <rle.h>
#include <my_bitstream.h> 
#include <stdint.h>
#include <huffman.h>
#include <htables.h>
#include <me_jpeg_writer.h>
#include <math.h>

/*
        Fait la différence terme à terme entre les termes successifs du vecteur.
*/
void difference(int32_t * matrix){
        for(int i = 1; i < BLOC_SIZE*BLOC_SIZE-1; i++){
                matrix[BLOC_SIZE*BLOC_SIZE-i] -= matrix[BLOC_SIZE*BLOC_SIZE-i-1];
        }
}
/*
        Renvoie l'indice de l'élèment value 
*/
int8_t in_array(int32_t * matrix, const uint8_t len, int32_t value)
{
        for(uint8_t i = 0; i < len; i++){
                if(matrix[i] == value){
                        return (int8_t)i;
                }
        }
        return -1;
}

/*
        Donne la magnétude de la valeur passée en paramètre.
*/
uint8_t magnetude(int32_t val)
{
        return log2(abs(val))+1;
}


uint16_t get_max(uint8_t indice_fort)
{
        uint16_t sum = 0;
        for(uint8_t i = 0; i <= indice_fort; i++)
        {
                sum += (uint16_t) pow(2, i);
        }
        return sum;
}

/*
        Donne l'indexe de l'entrée val.
*/
uint16_t get_index(int32_t val)
{
        if(val < 0)
        {
                uint16_t maxi = get_max(magnetude(val)-1);
                return maxi + val;
        }
        else
        {
                return val; 
        }
}

/*
        Renvoie l'indice du premier élement non à partir de start.
        le maximum est 17 (16 +1 car on envoie l'indice d'après d'après.)
*/
uint8_t longest_seq0(int32_t * matrix, uint8_t start){
        uint8_t i = start;
        while(i < BLOC_SIZE*BLOC_SIZE && i- start +1 != 17 && matrix[i] == 0){
                ++i;
        }
        return i;
}
/*
        Vérifie si le reste du fichier est composé que de 0. 
        Renvoie true si c'est le cas, false sinon.
*/
bool only_zero(int32_t * matrix, uint8_t start)
{
        uint8_t i = start;
        while(i < BLOC_SIZE*BLOC_SIZE && matrix[i] == 0){
                ++i;
        }
        return i == 64;
}

/*
        Encode les coefficients AC de la matrice "matrix". On donne la table de huffman associée ainsi 
        que le bitsream du fichier jpeg. 
*/
void encode_ac(struct my_bitstream * bs, int32_t *matrix, struct huff_table* ht)
{       
        uint8_t nb_bits_code;
        uint32_t code_mag;
        uint16_t bit_seq = 0;
        uint8_t end_seq = 0;
        uint16_t index;
        for(int i = 1; end_seq < BLOC_SIZE*BLOC_SIZE; i = end_seq){
                if(matrix[i] == 0){
                        if(only_zero(matrix, i))
                        {
                        bit_seq  = 0x00;
                        code_mag = huffman_table_get_path(ht, bit_seq, &nb_bits_code);
                        my_bitstream_write_bits(bs, code_mag, nb_bits_code);
                        end_seq = 64;
                        }
                        else
                        {
                                end_seq = longest_seq0(matrix, i);
                                if(end_seq - i == 16){
                                        bit_seq  = 0xF0;
                                        code_mag = huffman_table_get_path(ht, bit_seq, &nb_bits_code);
                                        my_bitstream_write_bits(bs, code_mag, nb_bits_code);

                                }
                                else if(end_seq == 64)
                                {
                                        bit_seq = 0x00;
                                        code_mag = huffman_table_get_path(ht, bit_seq, &nb_bits_code);
                                        my_bitstream_write_bits(bs, code_mag, nb_bits_code);
                                        my_bitstream_write_bits(bs, 0, magnetude(matrix[end_seq]));
                                }
                                else{
                                        bit_seq  = (end_seq -i)*16 + magnetude(matrix[end_seq]);  // pas de +1 car end_seq est derrière le dernier 0
                                        index = get_index(matrix[end_seq]);
                                        code_mag = huffman_table_get_path(ht, bit_seq, &nb_bits_code);

                                        my_bitstream_write_bits(bs, code_mag, nb_bits_code);
                                        my_bitstream_write_bits(bs, index, magnetude(matrix[end_seq]));
                                        end_seq++;
                                }
                        }
                }
                else{
                        end_seq = i;
                        bit_seq = magnetude(matrix[end_seq]); 
                        index = get_index(matrix[end_seq]);
                        code_mag = huffman_table_get_path(ht, bit_seq, &nb_bits_code);
                        my_bitstream_write_bits(bs, code_mag, nb_bits_code);
                        my_bitstream_write_bits(bs, index, magnetude(matrix[end_seq]));
                        end_seq++;
                }
        
        }
}

/*
        Encode le coefficient DC de la matrice. "matrix" est la valeur de ce DC. 
        La structure my_bitstream est celle de la structure jpeg.
*/
void encode_dc(struct my_bitstream * bs, int32_t matrix, struct huff_table* ht)
{
        uint16_t index = get_index(matrix);
        uint8_t mag =  magnetude(matrix);
        uint8_t nb_bits_code;
        uint32_t code_mag = huffman_table_get_path(ht, mag, &nb_bits_code);
        my_bitstream_write_bits(bs, code_mag, nb_bits_code);
        my_bitstream_write_bits(bs, index, mag);
}
