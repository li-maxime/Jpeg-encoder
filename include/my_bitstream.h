#ifndef MY_BITSTREAM_H
#define MY_BITSTREAM_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

struct my_bitstream;

/* 
    Retourne un nouveau my_bitstream prêt à écrire dans le fichier filename. 
*/
extern struct my_bitstream *my_bitstream_create(FILE *filename);

/*
    Ecrit nb_bits bits dans le my_bitstream. La valeur portée par cet ensemble de
    bits est value. Le paramètre is_marker permet d'indiquer qu'on est en train
    d'écrire un marqueur de section dans l'entête JPEG ou non (voir section
    "Encodage dans le flux JPEG -> Byte stuffing" du sujet).
*/
extern void my_bitstream_write_bits(struct my_bitstream *stream, uint32_t value, uint8_t nb_bits);
/*
    Force l'exécution des écritures en attente sur le my_bitstream, s'il en
    existe.
*/

extern void my_bitstream_flush(struct my_bitstream *stream);
/*
    Détruit le my_bitstream passé en paramètre, en libérant la mémoire qui lui est
    associée. Vie le buffer s'il n'est pas vide.
*/
extern void my_bitstream_destroy(struct my_bitstream *stream);

#endif /* MY_BITSTREAM_H */
