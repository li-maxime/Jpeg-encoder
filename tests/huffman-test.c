#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <huffman.h>
#include <htables.h>
#include <assert.h>

int main(){

    struct huff_table* ht_dc_y = huffman_table_build(htables_nb_symb_per_lengths[DC][Y], htables_symbols[DC][Y], htables_nb_symbols[DC][Y]);
    uint8_t nb_bits = 0;
    uint32_t path;

    path = huffman_table_get_path(ht_dc_y, 0, &nb_bits);
    assert(path == 0b00 && nb_bits == 2);
    path = huffman_table_get_path(ht_dc_y, 1, &nb_bits);
    assert(path == 0b010 && nb_bits == 3);
    path = huffman_table_get_path(ht_dc_y, 2, &nb_bits);
    assert(path == 0b011 && nb_bits == 3);
    path = huffman_table_get_path(ht_dc_y, 3, &nb_bits);
    assert(path == 0b100 && nb_bits == 3);
    path = huffman_table_get_path(ht_dc_y, 4, &nb_bits);
    assert(path == 0b101 && nb_bits == 3);
    path = huffman_table_get_path(ht_dc_y, 5, &nb_bits);
    assert(path == 0b110 && nb_bits == 3);
    path = huffman_table_get_path(ht_dc_y, 6, &nb_bits);
    assert(path == 0b1110 && nb_bits == 4);
    path = huffman_table_get_path(ht_dc_y, 7, &nb_bits);
    assert(path == 0b11110 && nb_bits == 5);
    path = huffman_table_get_path(ht_dc_y, 8, &nb_bits);
    assert(path == 0b111110 && nb_bits == 6);
    huffman_table_destroy(ht_dc_y);
    
    printf("Tous les tests pour la table DC_Luminance sont réussis !\n");

    struct huff_table* ht_dc_c = huffman_table_build(htables_nb_symb_per_lengths[DC][Cr], htables_symbols[DC][Cr], htables_nb_symbols[DC][Cr]);
    path = huffman_table_get_path(ht_dc_c, 0, &nb_bits);
    assert(path == 0b00 && nb_bits == 2);
    path = huffman_table_get_path(ht_dc_c, 1, &nb_bits);
    assert(path == 0b01 && nb_bits == 2);
    path = huffman_table_get_path(ht_dc_c, 2, &nb_bits);
    assert(path == 0b10 && nb_bits == 2);
    path = huffman_table_get_path(ht_dc_c, 3, &nb_bits);
    assert(path == 0b110 && nb_bits == 3);
    path = huffman_table_get_path(ht_dc_c, 4, &nb_bits);
    assert(path == 0b1110 && nb_bits == 4);
    path = huffman_table_get_path(ht_dc_c, 5, &nb_bits);
    assert(path == 0b11110 && nb_bits == 5);
    path = huffman_table_get_path(ht_dc_c, 6, &nb_bits);
    assert(path == 0b111110 && nb_bits == 6);
    path = huffman_table_get_path(ht_dc_c, 7, &nb_bits);
    assert(path == 0b1111110 && nb_bits == 7);
    path = huffman_table_get_path(ht_dc_c, 8, &nb_bits);
    assert(path == 0b11111110 && nb_bits == 8);
    huffman_table_destroy(ht_dc_c);

    printf("Tous les tests pour la table DC_Chrominance sont réussis !\n");

    struct huff_table* ht_ac_y = huffman_table_build(htables_nb_symb_per_lengths[AC][Y], htables_symbols[AC][Y], htables_nb_symbols[AC][Y]);
    path = huffman_table_get_path(ht_ac_y, 0x17, &nb_bits);
    assert(path == 0b1111111110000101 && nb_bits == 16);
    path = huffman_table_get_path(ht_ac_y, 0x11, &nb_bits);
    assert(path == 0b1100 && nb_bits == 4);
    path = huffman_table_get_path(ht_ac_y, 0xD3, &nb_bits);
    assert(path == 0b1111111111100011 && nb_bits == 16);
    path = huffman_table_get_path(ht_ac_y, 0xE9, &nb_bits);
    assert(path == 0b1111111111110011 && nb_bits == 16);
    path = huffman_table_get_path(ht_ac_y, 0x07, &nb_bits);
    assert(path == 0b11111000 && nb_bits == 8);
    path = huffman_table_get_path(ht_ac_y, 0x76, &nb_bits);
    assert(path == 0b1111111110110001 && nb_bits == 16);
    path = huffman_table_get_path(ht_ac_y, 0x03, &nb_bits);
    assert(path == 0b100 && nb_bits == 3);
    path = huffman_table_get_path(ht_ac_y, 0x00, &nb_bits);
    assert(path == 0b1010 && nb_bits == 4);
    path = huffman_table_get_path(ht_ac_y, 0x51, &nb_bits);
    assert(path == 0b1111010 && nb_bits == 7);
    huffman_table_destroy(ht_ac_y);

    printf("Tous les tests pour la table AC_Luminance sont réussis !\n");

    struct huff_table* ht_ac_c = huffman_table_build(htables_nb_symb_per_lengths[AC][Cr], htables_symbols[AC][Cr], htables_nb_symbols[AC][Cr]);
    path = huffman_table_get_path(ht_ac_c, 0x17, &nb_bits);
    assert(path == 0b1111111110001000 && nb_bits == 16);
    path = huffman_table_get_path(ht_ac_c, 0x11, &nb_bits);
    assert(path == 0b1011 && nb_bits == 4);
    path = huffman_table_get_path(ht_ac_c, 0xD3, &nb_bits);
    assert(path == 0b1111111111100101 && nb_bits == 16);
    path = huffman_table_get_path(ht_ac_c, 0xE9, &nb_bits);
    assert(path == 0b1111111111110100 && nb_bits == 16);
    path = huffman_table_get_path(ht_ac_c, 0x07, &nb_bits);
    assert(path == 0b1111000 && nb_bits == 7);
    path = huffman_table_get_path(ht_ac_c, 0x76, &nb_bits);
    assert(path == 0b1111111110110010 && nb_bits == 16);
    path = huffman_table_get_path(ht_ac_c, 0x03, &nb_bits);
    assert(path == 0b1010 && nb_bits == 4);
    path = huffman_table_get_path(ht_ac_c, 0x00, &nb_bits);
    assert(path == 0b00 && nb_bits == 2);
    path = huffman_table_get_path(ht_ac_c, 0x51, &nb_bits);
    assert(path == 0b111011 && nb_bits == 6);
    huffman_table_destroy(ht_ac_c);

    printf("Tous les tests pour la table AC_Chrominance sont réussis !\n");

    return EXIT_SUCCESS;
}
