#ifndef ME_JPEG_WRITER_H
#define ME_JPEG_WRITER_H

#include <stdint.h>
#include <stdio.h>

/********************/
/* Types de données */
/********************/
/* Type énuméré représentant les composantes de couleur YCbCr. */
enum color_component
{
    Y,
    Cb,
    Cr,
    NB_COLOR_COMPONENTS
};

/*
    Type énuméré représentant les types de composantes fréquentielles (DC ou AC).
*/
enum sample_type
{
    DC,
    AC,
    NB_SAMPLE_TYPES
};

/*
    Type énuméré représentant la direction des facteurs d'échantillonnage (H
    pour horizontal, V pour vertical).
*/
enum direction
{
    H,
    V,
    NB_DIRECTIONS
};

/*
    Type opaque contenant l'intégralité des informations
    nécessaires à l'écriture de l'en-tête me_JPEG.
*/
struct me_jpeg;

/* Type opaque représentant un arbre de Huffman. */
struct huff_table;


/***********************************************/
/* Ouverture, fermeture et fonctions générales */
/***********************************************/

/* Alloue et retourne une nouvelle structure me_jpeg. */
extern struct me_jpeg *me_jpeg_create(void);

/*
    Détruit une structure me_jpeg.
    Toute la mémoire qui lui est associée est libérée.
*/
extern void me_jpeg_destroy(struct me_jpeg *jpg);

/*
    Ecrit tout l'en-tête me_JPEG dans le fichier de sortie à partir des
    informations contenues dans la structure me_jpeg passée en paramètre.
    En sortie, le bitstream est positionné juste après l'écriture de
    l'en-tête SOS, à l'emplacement du premier octet de données brutes à écrire.
*/
extern void me_jpeg_write_header(struct me_jpeg *jpg);

extern void progressive_header(struct me_jpeg *jpg);

extern void progressive_SOS(FILE* fichier, uint8_t ssstart, uint8_t ssend, enum color_component cc, uint8_t dcac_table);
/* Ecrit le footer me_JPEG (marqueur EOI) dans le fichier de sortie. */
extern void me_jpeg_write_footer(struct me_jpeg *jpg);

extern void footer_progressive(FILE* fichier);
/*
    Retourne le bitstream associé au fichier de sortie enregistré
    dans la structure me_jpeg.
*/
extern struct my_bitstream *me_jpeg_get_bitstream(struct me_jpeg *jpg);


/****************************************************/
/* Gestion des paramètres de l'encodeur via le me_jpeg */
/****************************************************/

/* Ecrit le nom de fichier PPM ppm_filename dans la structure me_jpeg. */
extern void me_jpeg_set_ppm_filename(struct me_jpeg *jpg,
                                  const char *ppm_filename);

/* Retourne le nom de fichier PPM lu dans la structure me_jpeg. */
extern char *me_jpeg_get_ppm_filename(struct me_jpeg *jpg);

/* Ecrit le nom du fichier de sortie me_jpeg_filename dans la structure me_jpeg. */
extern void me_jpeg_set_me_jpeg_filename(struct me_jpeg *jpg,
                                   const char *me_jpeg_filename);

/* Retourne le nom du fichier de sortie lu depuis la structure me_jpeg. */
extern char *me_jpeg_get_me_jpeg_filename(struct me_jpeg *jpg);


/*
    Ecrit la hauteur de l'image traitée, en nombre de pixels,
    dans la structure me_jpeg.
*/
extern void me_jpeg_set_image_height(struct me_jpeg *jpg,
                                  uint32_t image_height);

/*
    Retourne la hauteur de l'image traitée, en nombre de pixels,
    lue dans la structure me_jpeg.
*/
extern uint32_t me_jpeg_get_image_height(struct me_jpeg *jpg);

/*
    Ecrit la largeur de l'image traitée, en nombre de pixels,
    dans la structure me_jpeg.
*/
extern void me_jpeg_set_image_width(struct me_jpeg *jpg,
                                 uint32_t image_width);

/*
    Retourne la largeur de l'image traitée, en nombre de pixels,
    lue dans la structure me_jpeg.
*/
extern uint32_t me_jpeg_get_image_width(struct me_jpeg *jpg);


/*
    Ecrit le nombre de composantes de couleur de l'image traitée
    dans la structure me_jpeg.
*/
extern void me_jpeg_set_nb_components(struct me_jpeg *jpg,
                                   uint8_t nb_components);

/*
    Retourne le nombre de composantes de couleur de l'image traitée
    lu dans la structure me_jpeg.
*/
extern uint8_t me_jpeg_get_nb_components(struct me_jpeg *jpg);


/*
    Ecrit dans la structure me_jpeg le facteur d'échantillonnage sampling_factor
    à utiliser pour la composante de couleur cc et la direction dir.
*/
extern void me_jpeg_set_sampling_factor(struct me_jpeg *jpg,
                                     enum color_component cc,
                                     enum direction dir,
                                     uint8_t sampling_factor);

/*
    Retourne le facteur d'échantillonnage utilisé pour la composante
    de couleur cc et la direction dir, lu dans la structure me_jpeg.
*/
extern uint8_t me_jpeg_get_sampling_factor(struct me_jpeg *jpg,
                                        enum color_component cc,
                                        enum direction dir);


/*
    Ecrit dans la structure me_jpeg la table de Huffman huff_table à utiliser
    pour encoder les données de la composante fréquentielle acdc, pour la
    composante de couleur cc.
*/
extern void me_jpeg_set_huffman_table(struct me_jpeg *jpg,
                                   enum sample_type acdc,
                                   enum color_component cc,
                                   struct huff_table *htable);

/*
    Retourne un pointeur vers la table de Huffman utilisée pour encoder
    les données de la composante fréquentielle acdc pour la composante
    de couleur cc, lue dans la structure me_jpeg.
*/
extern struct huff_table *me_jpeg_get_huffman_table(struct me_jpeg *jpg,
                                                 enum sample_type acdc,
                                                 enum color_component cc);


/*
    Ecrit dans la structure me_jpeg la table de quantification à utiliser
    pour compresser les coefficients de la composante de couleur cc.
*/
extern void me_jpeg_set_quantization_table(struct me_jpeg *jpg,
                                        enum color_component cc,
                                        uint8_t *qtable);

/*
    Retourne un pointeur vers la table de quantification associée à la
    composante de couleur cc, lue dans a structure me_jpeg.
*/
extern uint8_t *me_jpeg_get_quantization_table(struct me_jpeg *jpg,
                                            enum color_component cc);


extern void concatenate_file(struct me_jpeg *jpg, FILE* second);

#endif /* ME_JPEG_WRITER_H */
