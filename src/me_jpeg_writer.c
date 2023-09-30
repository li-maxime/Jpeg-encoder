#include <stdlib.h>
#include <me_jpeg_writer.h>
#include <my_bitstream.h>
#include <huffman.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

struct me_jpeg{
    FILE* fichier;
    char *ppm_filename;
    char *me_jpeg_filename;
    uint32_t image_height;
    uint32_t image_width;
    uint8_t nb_components;
    uint8_t h1;
    uint8_t h2;
    uint8_t h3;
    uint8_t v1;
    uint8_t v2;
    uint8_t v3;
    struct huff_table *Y_DC;
    struct huff_table *Y_AC;
    struct huff_table *C_DC;
    struct huff_table *C_AC;
    uint8_t *Y_qtab;
    uint8_t *C_qtab;
    struct my_bitstream* stream;

};



/* Alloue et retourne une nouvelle structure me_jpeg. */
struct me_jpeg *me_jpeg_create(void){
    struct me_jpeg *jpg = malloc(sizeof(struct me_jpeg));
    /*
    jpg->Y_AD = malloc(sizeof(huff_table));
    jpg->Y_AC = malloc(sizeof(huff_table));
    jpg->C_AD = malloc(sizeof(huff_table));
    jpg->C_AC = malloc(sizeof(huff_table));
    */
    return jpg;
}

/*
    Détruit une structure me_jpeg.
    Toute la mémoire qui lui est associée est libérée.
*/
void me_jpeg_destroy(struct me_jpeg *jpg){

    fclose(jpg->fichier);
    free(jpg->ppm_filename);
    free(jpg->me_jpeg_filename);
    free(jpg);
}

/*
    Ecriture header Application data
*/
static void header_APPx(struct me_jpeg *jpg){
    uint8_t buffer[]={0xff, 0xe0, 0, 16, 0x4A, 0x46, 0x49, 0x46, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0};
    fwrite(&buffer, sizeof(buffer), 1, jpg->fichier);

}

/*
    Ecriture header Commentaire prof
*/
static void header_COM(struct me_jpeg *jpg){
    uint8_t buffer[]={0xff, 0xfe, 00 ,0x10  ,0x3c ,0x33 ,0x20 ,0x6c ,0x65 ,0x20 ,0x70 ,0x72 ,0x6f ,0x6a ,0x65 ,0x74 ,0x20 ,0x43};
    fwrite(&buffer, sizeof(buffer), 1, jpg->fichier);

}

/*
    Ecriture header Define Quantization Table.
    Definie seulement la table 0 si le nombre de composante couleur est 1 
    et la table 0 et 1 si le nombre de composante est 3.
*/
static void header_DQT(struct me_jpeg *jpg){
    uint8_t buffer[]={0xff, 0xdb, 0, 67, 00};
    fwrite(&buffer, sizeof(buffer), 1, jpg->fichier);
    fwrite(jpg->Y_qtab, 64, 1, jpg->fichier);
    if (jpg->nb_components == 3){
        uint8_t buffer[]={0xff, 0xdb, 0, 67, 01};
        fwrite(&buffer, sizeof(buffer), 1, jpg->fichier);
        fwrite(jpg->C_qtab, 64, 1, jpg->fichier);
    }
}

/*
    Ecriture header Start of Scan.
    Le type vaut 2 pour progressive, dans le cas contraire on a un baseline
*/
static void header_SOFx(struct me_jpeg *jpg, uint8_t type){
    uint8_t marker = (type == 2) ? 0xc2 : 0xc0;
    uint8_t height_msb = jpg->image_height / 256;
    uint8_t height_lsb = jpg->image_height % 256;
    uint8_t width_msb = jpg->image_width / 256;
    uint8_t width_lsb = jpg->image_width % 256;
    uint8_t h1v1 = jpg->h1 * 16 + jpg->v1;
    if (jpg->nb_components == 1){
        uint8_t buffer[]={0xff, marker, 0, 11, 8, height_msb, height_lsb, width_msb, width_lsb, 1, 1, h1v1, 0 };
        fwrite(&buffer, sizeof(buffer), 1, jpg->fichier);
    }
    else{
        
        uint8_t h2v2 = jpg->h2 * 16 + jpg->v2;
        uint8_t h3v3 = jpg->h3 * 16 + jpg->v3;
        uint8_t buffer[]={0xff, marker, 0, 17, 8, height_msb, height_lsb, width_msb, width_lsb, 3, 1, h1v1, 0 , 2, h2v2, 1, 3, h3v3, 1 };
        fwrite(&buffer, sizeof(buffer), 1, jpg->fichier);
    }
}


/*
static uint16_t count_element_DHT(uint8_t *lenght_vector){
    uint16_t counter = 0;
    for (uint8_t i = 0; i<16; i++){
        counter += lenght_vector[i];
    }
    counter += 19;
    return counter;
}
*/

/*
    Ecriture header Define Huffman Table.
    Le premier hex de type indique DC/AC et le second indique Y ou Cb/Cr
*/
static void header_DHT(struct me_jpeg *jpg, uint8_t type){
    struct huff_table *htable = NULL;
    switch(type){
        case 0x0:
            htable = jpg->Y_DC;
            break;
        case 0x01:
            htable = jpg->C_DC;
            break;
        case 0x10:
            htable = jpg->Y_AC;
            break;
        case 0x11:
            htable = jpg->C_AC;
            break;
    }
    uint8_t *lenght_vector = huffman_table_get_length_vector(htable);
    uint8_t *symbols = huffman_table_get_symbols(htable);
    uint16_t counter = (uint16_t) (huffman_get_nb_symbols(htable)) + 19;
    uint8_t counter_msb = counter / 256;
    uint8_t counter_lsb = counter % 256;
    uint8_t buffer[]={0xff, 0xc4, counter_msb, counter_lsb, type};
    fwrite(&buffer, sizeof(buffer), 1, jpg->fichier);
    fwrite(lenght_vector, 16, 1, jpg->fichier);
    fwrite(symbols, counter - 19, 1, jpg->fichier);

}

/*
    Ecriture header Start of Scan
    ssstart indique le début de la selection spectrale
    ssend indique la fin de la selection
*/
static void header_SOS(struct me_jpeg *jpg, uint8_t ssstart, uint8_t ssend){
    if (jpg->nb_components == 1){
        uint8_t buffer[]={0xff, 0xda, 0, 8, 1, 1, 0, ssstart, ssend, 0 };
        fwrite(&buffer, sizeof(buffer), 1, jpg->fichier);
    }
    else{
        uint8_t buffer[]={0xff, 0xda, 0, 12, 3, 1, 0, 2, 0x11, 3, 0x11, ssstart, ssend, 0 };
        fwrite(&buffer, sizeof(buffer), 1, jpg->fichier);
    }
    

}


/*
    Ecrit tout l'en-tête me_jpeg dans le fichier de sortie à partir des
    informations contenues dans la structure me_jpeg passée en paramètre.
    En sortie, le bitstream est positionné juste après l'écriture de
    l'en-tête SOS, à l'emplacement du premier octet de données brutes à écrire.
*/
void me_jpeg_write_header(struct me_jpeg *jpg){
    jpg->fichier = fopen(jpg->me_jpeg_filename, "wb");
    uint16_t buffer[20];
    buffer[0] = 0xd8ff;
    fwrite(&buffer, 2, 1, jpg->fichier);
    header_APPx(jpg);
    header_COM(jpg);
    header_DQT(jpg);
    header_SOFx(jpg, 0);
    header_DHT(jpg, 0x00);
    header_DHT(jpg, 0x10);
    if (jpg->nb_components == 3){
        header_DHT(jpg, 0x01);
        header_DHT(jpg, 0x11);
    }
    header_SOS(jpg, 0x0, 0x3f); // ?
    jpg->stream = my_bitstream_create(jpg->fichier);
}

void progressive_header(struct me_jpeg *jpg){
    jpg->fichier = fopen(jpg->me_jpeg_filename, "wb");
    uint16_t buffer[20];
    buffer[0] = 0xd8ff;
    fwrite(&buffer, 2, 1, jpg->fichier);
    header_APPx(jpg);
    header_COM(jpg);
    header_DQT(jpg);
    header_SOFx(jpg, 2);
    header_DHT(jpg, 0x00);
    header_DHT(jpg, 0x10);
    if (jpg->nb_components == 3){
        header_DHT(jpg, 0x01);
        header_DHT(jpg, 0x11);
    }
    uint8_t buffer2[]={0xff, 0xda, 0, 8, 1, 1, 0, 0, 0, 0 };
    fwrite(&buffer2, sizeof(buffer2), 1, jpg->fichier);

    jpg->stream = my_bitstream_create(jpg->fichier);

}

void progressive_SOS(FILE* fichier, uint8_t ssstart, uint8_t ssend, enum color_component cc, uint8_t dcac_table){
    //header_DHT(jpg, 0x10);
    uint8_t buffer[]={0xff, 0xda, 0, 8, 1, cc + 1, dcac_table, ssstart, ssend, 0 };
    fwrite(&buffer, sizeof(buffer), 1, fichier);


}

/* Ecrit le footer me_jpeg (marqueur EOI) dans le fichier de sortie. */
void me_jpeg_write_footer(struct me_jpeg *jpg){
    my_bitstream_destroy(jpg->stream);
    int buffer[] = {0xd9ff};
    fwrite(&buffer, 2, 1, jpg->fichier);
}

void footer_progressive(FILE* fichier){
    int buffer[] = {0xd9ff};
    fwrite(&buffer, 2, 1,fichier);
}
/*
    Retourne le bitstream associé au fichier de sortie enregistré
    dans la structure me_jpeg.
*/
struct my_bitstream *me_jpeg_get_bitstream(struct me_jpeg *jpg){
    return jpg->stream;
}

/****************************************************/
/* Gestion des paramètres de l'encodeur via le me_jpeg */
/****************************************************/

/* Ecrit le nom de fichier PPM ppm_filename dans la structure me_jpeg. */
void me_jpeg_set_ppm_filename(struct me_jpeg *jpg, const char *ppm_filename){
    jpg->ppm_filename = malloc((strlen(ppm_filename)+1)*sizeof(char));
    strcpy(jpg->ppm_filename, ppm_filename);
}

/* Ecrit le nom du fichier de sortie me_jpeg_filename dans la structure me_jpeg. */
void me_jpeg_set_me_jpeg_filename(struct me_jpeg *jpg, const char *me_jpeg_filename){
    jpg->me_jpeg_filename = malloc((strlen(me_jpeg_filename)+1)*sizeof(char));
    strcpy(jpg->me_jpeg_filename, me_jpeg_filename);
}

/* Retourne le nom du fichier de sortie lu depuis la structure me_jpeg. */
char *me_jpeg_get_me_jpeg_filename(struct me_jpeg *jpg){
    return jpg->me_jpeg_filename;
}


/*
    Ecrit la hauteur de l'image traitée, en nombre de pixels,
    dans la structure me_jpeg.
*/
void me_jpeg_set_image_height(struct me_jpeg *jpg, uint32_t image_height){
    jpg->image_height = image_height;
}

/*
    Retourne la hauteur de l'image traitée, en nombre de pixels,
    lue dans la structure me_jpeg.
*/
uint32_t me_jpeg_get_image_height(struct me_jpeg *jpg){
    return jpg->image_height;
}

/*
    Ecrit la largeur de l'image traitée, en nombre de pixels,
    dans la structure me_jpeg.
*/
void me_jpeg_set_image_width(struct me_jpeg *jpg, uint32_t image_width){
    jpg->image_width = image_width;
}

/*
    Retourne la largeur de l'image traitée, en nombre de pixels,
    lue dans la structure me_jpeg.
*/
uint32_t me_jpeg_get_image_width(struct me_jpeg *jpg){
    return jpg->image_width;
}


/*
    Ecrit le nombre de composantes de couleur de l'image traitée
    dans la structure me_jpeg.
*/
void me_jpeg_set_nb_components(struct me_jpeg *jpg, uint8_t nb_components){
    jpg->nb_components = nb_components;
}

/*
    Retourne le nombre de composantes de couleur de l'image traitée
    lu dans la structure me_jpeg.
*/
uint8_t me_jpeg_get_nb_components(struct me_jpeg *jpg){
    return jpg->nb_components;
}


/*
    Ecrit dans la structure me_jpeg le facteur d'échantillonnage sampling_factor
    à utiliser pour la composante de couleur cc et la direction dir.
*/
void me_jpeg_set_sampling_factor(struct me_jpeg *jpg, enum color_component cc, enum direction dir, uint8_t sampling_factor){
    if (dir == H){
        if (cc == Y){
            jpg->h1 = sampling_factor;
        }
        else if (cc == Cb){
            jpg->h2 = sampling_factor;
        }
        else if (cc == Cr){
            jpg->h3 = sampling_factor;
        }
        else{
            printf("Erreur set_sampling : Entrée non valide");
        }
    }
    else if (dir == V)
    {
        if (cc == Y){
            jpg->v1 = sampling_factor;
        }
        else if (cc == Cb){
            jpg->v2 = sampling_factor;
        }
        else if (cc == Cr){
            jpg->v3 = sampling_factor;
        }
        else{
            printf("Erreur set_sampling : Entrée non valide");
        }
    }
    else {
        printf("Erreur set_sampling : Entrée non valide");
    }
}

/*
    Retourne le facteur d'échantillonnage utilisé pour la composante
    de couleur cc et la direction dir, lu dans la structure me_jpeg.
*/
uint8_t me_jpeg_get_sampling_factor(struct me_jpeg *jpg, enum color_component cc, enum direction dir){
    if (dir == H){
        if (cc == Y){
            return jpg->h1;
        }
        else if (cc == Cb){
            return jpg->h2;
        }
        else if (cc == Cr){
            return jpg->h3;
        }
        else{
            printf("Erreur get_sampling : Entrée non valide");
        }
    }
    else if (dir == V)
    {
        if (cc == Y){
            return jpg->v1;
        }
        else if (cc == Cb){
            return jpg->v2;
        }
        else if (cc == Cr){
            return jpg->v3;
        }
        else{
            printf("Erreur get_sampling : Entrée non valide");
        }
    }
    else {
        printf("Erreur get_sampling : Entrée non valide");
    }
    return 0;
}


/*
    Ecrit dans la structure me_jpeg la table de Huffman huff_table à utiliser
    pour encoder les données de la composante fréquentielle acdc, pour la
    composante de couleur cc.
*/
void me_jpeg_set_huffman_table(struct me_jpeg *jpg, enum sample_type acdc, enum color_component cc, struct huff_table *htable){
    if (acdc == DC){
        if (cc == Y){
            jpg->Y_DC = htable;
        } 
        else{
            jpg->C_DC = htable;
        }
    }
    else if (acdc == AC){
        if (cc == Y){
            jpg->Y_AC = htable;
        } 
        else{
            jpg->C_AC = htable;
        }   
    }
    else{
        printf("Erreur get_huffman_tables : Entrée non valide");
    }
}

/*
    Retourne un pointeur vers la table de Huffman utilisée pour encoder
    les données de la composante fréquentielle acdc pour la composante
    de couleur cc, lue dans la structure me_jpeg.
*/
struct huff_table *me_jpeg_get_huffman_table(struct me_jpeg *jpg, enum sample_type acdc, enum color_component cc){
    if (acdc == DC){
        if (cc == Y){
            return jpg->Y_DC;
        } 
        else{
            return jpg->C_DC;
        }
    }
    else if (acdc == AC){
        if (cc == Y){
            return jpg->Y_AC;
        } 
        else{
            return jpg->C_AC;
        }   
    }
    else{
        printf("Erreur get_huffman_tables : Entrée non valide");
        return 0;
    }
}


/*
    Ecrit dans la structure me_jpeg la table de quantification à utiliser
    pour compresser les coefficients de la composante de couleur cc.
*/
void me_jpeg_set_quantization_table(struct me_jpeg *jpg, enum color_component cc, uint8_t *qtable){
    if (cc == Y){
        jpg->Y_qtab = qtable;
    }
    else{
        jpg->C_qtab = qtable;
    }
}

/*
    Retourne un pointeur vers la table de quantification associée à la
    composante de couleur cc, lue dans a structure me_jpeg.
*/
uint8_t *me_jpeg_get_quantization_table(struct me_jpeg *jpg, enum color_component cc){
    if (cc == Y){
        return jpg->Y_qtab;
    }
    else{
        return jpg->C_qtab;
    }
}


void concatenate_file(struct me_jpeg *jpg, FILE* second){
    char buffer[5000];
    fseek(second, 0, SEEK_SET);
    uint16_t size = fread(buffer, 1, 5000, second);
    while(size != 0){
        fwrite(buffer, size, 1, jpg->fichier);
        size = fread(buffer, 1, 5000, second);
    }
    fclose(second);
}

/*

int main(void){
    
    FILE* fichier;
    struct me_jpeg *jpg = malloc(sizeof(struct me_jpeg));
    jpg->jpg_filename = malloc((strlen(argv[1])+1)*sizeof(char));
    strcpy(jpg->ppm_filename, argv[1]);
    fichier = fopen(jpg->, "wb");
    unsigned int i;
    i = strtol("ABCDEF123456789", NULL, 16);
    
    fwrite(&i, 10, 1, fichier);
    fclose(fichier);
    
    struct me_jpeg *jpg = me_jpeg_create();
    me_jpeg_set_me_jpeg_filename(jpg, "test_header.bin");
    me_jpeg_set_quantization_table(jpg, 0, quantification_table_Y);
    me_jpeg_set_quantization_table(jpg, 1, quantification_table_CbCr);
    me_jpeg_set_nb_components(jpg, 3);
    me_jpeg_set_image_height(jpg, 1246);
    me_jpeg_set_image_width(jpg, 704);
    me_jpeg_set_sampling_factor(jpg, 0, 0, 2);
    me_jpeg_set_sampling_factor(jpg, 0, 1, 2);
    me_jpeg_set_sampling_factor(jpg, 1, 0, 1);
    me_jpeg_set_sampling_factor(jpg, 1, 1, 2);
    me_jpeg_set_sampling_factor(jpg, 2, 0, 2);
    me_jpeg_set_sampling_factor(jpg, 2, 1, 1);
    me_jpeg_write_header(jpg);
    me_jpeg_write_footer(jpg);
    //me_jpeg_destroy(jpg);
    
    return EXIT_SUCCESS;
}

*/