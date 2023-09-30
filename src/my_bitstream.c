#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <my_bitstream.h>


#define LEN_BUFFER 64
/*
    Type opaque représentant le flux d'octets à écrire dans le fichier JPEG de
    sortie (appelé my_bitstream dans le sujet).
*/
struct my_bitstream{
        FILE *file;
        uint8_t *bytes;
        uint64_t  buffer;
        uint8_t nb_bits_use;
};

/*
    Permet de trancher les "place_restante" bit de la représentation binaire de value sur nb_bit.
    Cera utilisé pour combler un buffer qui n'aurait pas assez de place pour acceuillir tous les bits.
*/
uint32_t cut_value(uint32_t* value, uint8_t nb_bit, uint8_t place_restante){
    uint32_t tronc_seq = 0;
    uint32_t pow_of_2 = (uint32_t) pow(2, nb_bit-place_restante);
    tronc_seq = *value / pow_of_2;
    *value -= (uint32_t) (tronc_seq * pow_of_2);
    //printf("head : %u \n", tronc_seq);
    //printf("queue : %u \n", *value);
    return tronc_seq;
}

/* 
    Retourne un nouveau my_bitstream prêt à écrire dans le fichier file. 
*/
struct my_bitstream *my_bitstream_create(FILE *file){
        struct my_bitstream* stream = (struct my_bitstream*) malloc(sizeof(struct my_bitstream));
        stream->buffer = 0;
        stream->file = file;
        stream->nb_bits_use = 0;
        stream->bytes = calloc(16, sizeof(uint8_t));
        return stream;
}

/*
    Ecrit nb_bits bits dans le my_bitstream. La valeur portée par cet ensemble de
    bits est value. Le paramètre is_marker permet d'indiquer qu'on est en train
    d'écrire un marqueur de section dans l'entête JPEG ou non (voir section
    "Encodage dans le flux JPEG -> Byte stuffing" du sujet).
*/
uint8_t reverse(struct my_bitstream *bitstream){
    uint64_t val = bitstream->buffer;
    uint8_t *tab = bitstream->bytes;
    uint8_t zero = 0;
    for(uint8_t i = 0; i <8; i++){
        tab[i+zero] = val>>56 ;
        //printf("octet : %x\n\n", tab[i+zero]);
        if (val>>56 == 0xff){
            zero ++;
            tab[i + zero] = 0;
            //printf("octet : %x\n\n", tab[i+zero]);
        }
        val = val << 8;
        
    }
    return zero;

    
}

/*
    Cette fonction permet d'écrire dans le fichier. On remplie un buffer, une fois plein écrit son contenue 
    dans le fichier et on le vide.
    Le buffer permet de reduire le nombre d'appel de la fonction fwrite, l'écriture dans le fichier est donc plus efficace. 
*/
void my_bitstream_write_bits(struct my_bitstream *stream, uint32_t value, uint8_t nb_bits){                   
        if(LEN_BUFFER - stream->nb_bits_use == 0){
            uint8_t zero = reverse(stream);
         
            fwrite((stream->bytes), 8 +zero , 1, stream->file);
            stream->buffer = (uint64_t) value ;
            stream->nb_bits_use = nb_bits;
        }
        else if(LEN_BUFFER - stream->nb_bits_use < nb_bits){             
            uint32_t queue = value;

            uint32_t head = cut_value(&queue, nb_bits, LEN_BUFFER - stream->nb_bits_use);

            stream->buffer = stream->buffer * ((uint64_t) pow(2, LEN_BUFFER - stream->nb_bits_use)) + head;

            uint8_t zero = reverse(stream);
            fwrite((stream->bytes), 8+zero, 1, stream->file);

            stream->buffer = (uint64_t) queue ;
            
            stream->nb_bits_use = nb_bits - (LEN_BUFFER - stream->nb_bits_use);
        }
        else {
            stream->buffer = stream->buffer * ((uint64_t) pow(2, nb_bits)) + value;
            stream->nb_bits_use += nb_bits;
        }
}
/*
    Force l'exécution des écritures en attente sur le my_bitstream, s'il en
    existe.
*/

void my_bitstream_flush(struct my_bitstream *stream)
{
    if(stream->nb_bits_use != 0){
        stream->buffer = stream->buffer << (64-stream->nb_bits_use);
        uint8_t zero = reverse(stream);
        uint8_t nb_bits = (stream->nb_bits_use + 7)/8 +zero;
        fwrite((stream->bytes), nb_bits, 1, stream->file);//stream->nb_bits_use%8
        stream->buffer = 0;
        stream->nb_bits_use = 0;
    }
}

/*
    Détruit le my_bitstream passé en paramètre, en libérant la mémoire qui lui est
    associée. Vide le buffer s'il n'est pas vide.
*/
void my_bitstream_destroy(struct my_bitstream *stream)
{       
        //printf("détruit\n");
        my_bitstream_flush(stream);
        free(stream->bytes);
        free(stream);
}

/*
int main()
{
 
    FILE * file;
    file = fopen("try_my_bitstream.txt", "w");
    struct my_bitstream * stream =  my_bitstream_create(file);
    my_bitstream_write_bits(stream, 30,  5, false);
    my_bitstream_write_bits(stream, 90,  7, false);
    my_bitstream_write_bits(stream, 0,  2, false);
    my_bitstream_write_bits(stream, 1,  1, false);
    my_bitstream_write_bits(stream, 26,  5, false);





    my_bitstream_destroy(stream);
    fclose(file);
  
    return EXIT_SUCCESS;
}
*/
