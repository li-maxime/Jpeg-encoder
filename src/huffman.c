#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <huffman.h>

/*
    Arbre de Huffman.
    *symbols            ->  Tableau des symboles ordonnés dans l'arbre
    *length_vector      ->  Tableau du nb de symboles par longueur
    *length_per_symbol  ->  Tableau de longueurs dont indice correspond aux symboles
    nb_symbols          ->  Nombre de symboles du tableau
    *codes_vector       ->  Tableau des codes par ordre des symboles

*/
struct huff_table {
    uint8_t *symbols;
    uint8_t *length_vector;
    uint8_t *length_per_symbol;
    uint8_t nb_symbols;
    uint32_t *codes_vector;
};

/*
    Construit arbre de Huffman à partir des données
    passées en argument.
    *nb_symb_per_lengths    :  Tableau contenant le nombre
    de symboles pour chaque longueur de 1 à 16.
    symbols                 :  Tableau des symboles ordonnés
    nb_symbols              :  Taille du tableau symbols.
*/
struct huff_table *huffman_table_build(uint8_t *nb_symb_per_lengths,
                                        uint8_t *symbols,
                                        uint8_t nb_symbols)
{
    struct huff_table *ht = malloc(sizeof(struct huff_table));
    ht->length_per_symbol = malloc(sizeof(uint8_t)*nb_symbols);
    ht->codes_vector = malloc(sizeof(uint32_t)*nb_symbols);

    uint8_t k = 0;
    uint32_t c = 0;
    uint8_t code_length = nb_symb_per_lengths[0];
    uint8_t m = 1;
    uint8_t first = 0;


    for (uint8_t i = 0; i < 16; i++){
        code_length = nb_symb_per_lengths[i];

        if (code_length != 0){

            c = m*(c+1)*first;
            first = 1;

            for (uint8_t j = 0; j < code_length; j++){
                ht->codes_vector[k] = c;
                //printf("%d ", codes_vector[k]);
                ht->length_per_symbol[k] = i+1;
                c++;
                k++;
            }

            c--;
            m = 1;
        }

        m = 2*m;

    }

    ht->symbols = symbols;
    ht->length_vector = nb_symb_per_lengths;
    ht->nb_symbols = nb_symbols;


    return ht;
}

/*
   Retourne le tableau des symboles associé à l'arbre de
   Huffman passé en paramètre.
*/
uint8_t *huffman_table_get_symbols(struct huff_table *ht)
{
    return ht->symbols;
}

/*
    Retourne le tableau du nombre de symboles de chaque longueur
    associé à l'arbre de Huffman passé en paramètre.
*/
uint8_t *huffman_table_get_length_vector(struct huff_table *ht)
{
    return ht->length_vector;
}

/*
    Retourne le chemin dans l'arbre ht permettant d'atteindre
    la feuille de valeur value. nb_bits est un paramètre de sortie
    permettant de stocker la longueur du chemin retourné.
    *ht                     :  Arbre binaire
    value                   :  Valeur cherchée
    *nb_bits                :  Longueur du chemin retourné. 
*/
uint32_t huffman_table_get_path(struct huff_table *ht,
                                       uint8_t value,
                                       uint8_t *nb_bits)
{
    uint8_t i = 0;
    while (ht->symbols[i] != value && i < ht->nb_symbols){
        i++;
    }

    if (i == ht->nb_symbols){
        fprintf(stderr, "Value doesn't exist in Huffman table !");
        return 0;
    }

    *nb_bits = ht->length_per_symbol[i];

    return (ht->codes_vector[i]);
}

/*
    Retourne nombre de symboles.
*/
uint8_t huffman_get_nb_symbols(struct huff_table *ht)
{
    return ht->nb_symbols;
}

/*
    Détruit l'arbre de Huffman passé en paramètre et libère
    toute la mémoire qui lui est associée.
*/
void huffman_table_destroy(struct huff_table *ht)
{
    free(ht->length_per_symbol);
    free(ht->codes_vector);
    free(ht);
}
