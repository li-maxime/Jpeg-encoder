#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <me_jpeg_writer.h>
#include <ppm_reader.h>
#include <qtables.h>
#include <dct.h>
#include <mcu.h>
#include <rle.h>
#include <htables.h>
#include <huffman.h>
#include <downsampling.h>
#include <progressive.h>
#include <string.h>
/*
    Cette fonction est l'encodeur jpeg à proprement parlé. Elle effectue toutes les étapes de la compression jpeg.
*/
void ppm2jpeg(uint8_t h1, uint8_t v1, uint8_t h2, uint8_t v2, 
              uint8_t h3, uint8_t v3, char *filename, char *outfile, uint8_t dct_type)
{
        //On met un pointeur de fonction vers la bonne fonction dct. Pour éviter d'évaluer la condition (dct_type == 0) à chaque bloc.
        void (* dct_function)(int32_t *, int32_t *) = NULL;
        if(dct_type == 0){
            dct_function = &dct_2;
        }
        else{
            dct_function = &loeffler;
        }

        //Creation de la structure jpeg.
        struct image* img = convert_file(filename, h1, v1);
        struct me_jpeg* file = me_jpeg_create();

        
        me_jpeg_set_ppm_filename(file, filename);
        me_jpeg_set_me_jpeg_filename(file, outfile);
        me_jpeg_set_image_height(file, img->row);
        me_jpeg_set_image_width(file, img->col);

        //On différencie couleur et niveau de gris.
        if(img->type==5){
                
                me_jpeg_set_nb_components(file, 1);
                me_jpeg_set_sampling_factor(file, Y, H, 1);
                me_jpeg_set_sampling_factor(file, Y, V, 1);

                //Construction des tables ded huffman.
                struct huff_table* ht_dc = huffman_table_build(htables_nb_symb_per_lengths[DC][Y], htables_symbols[DC][Y], htables_nb_symbols[DC][Y]);
                me_jpeg_set_huffman_table(file, DC, Y, ht_dc);
                

                struct huff_table* ht_ac = huffman_table_build(htables_nb_symb_per_lengths[AC][Y], htables_symbols[AC][Y], htables_nb_symbols[AC][Y]);
                me_jpeg_set_huffman_table(file, AC, Y, ht_ac);
                
                //On donne la table de quatification.
                me_jpeg_set_quantization_table(file, Y, quantification_table_Y);
                
                //Ecriture de l'entête.
                me_jpeg_write_header(file);
                
                // On récupère le bitsream de notre de structure.
                struct my_bitstream* brute_stream = me_jpeg_get_bitstream(file);


                //debut traitement des mcu
                int32_t* spatial_bloc = NULL;
                int32_t* frequential_bloc = NULL;
                int32_t* auxiliaire =  NULL;
                int32_t last_dc = 0;
                frequential_bloc = (int32_t*) calloc(64, sizeof(int32_t));

                next_matrix(img);
                bool conti = true;
                while(conti){
                        conti = new_mcu(img);
                        spatial_bloc = img_get_current_mcu(img);
                       
                        for(int i = 0; i < 64; i++ ){
                                spatial_bloc[i] -= 128;
                        }
                        
                        dct_function(spatial_bloc, frequential_bloc);

                        auxiliaire = reorganization_zigzag(frequential_bloc, quantification_table_Y);
                        free(frequential_bloc);
                        frequential_bloc = auxiliaire;

                        encode_dc(brute_stream,frequential_bloc[0]-last_dc,ht_dc);
                        //on stock les DC à chaque itération, pour la prochaine itération.  
                        last_dc = frequential_bloc[0];

                        encode_ac(brute_stream,frequential_bloc,ht_ac);
                }
              
                free(frequential_bloc);
                huffman_table_destroy(ht_dc);
                huffman_table_destroy(ht_ac);
        }
        else{
                me_jpeg_set_nb_components(file, 3);
                
                //Composante Y
                me_jpeg_set_sampling_factor(file, Y, H, h1);
                me_jpeg_set_sampling_factor(file, Y, V, v1);
                //Composante Cb
                me_jpeg_set_sampling_factor(file, Cb, H, h2);
                me_jpeg_set_sampling_factor(file, Cb, V, v2);
                //Composante Cr
                me_jpeg_set_sampling_factor(file, Cr, H, h3);
                me_jpeg_set_sampling_factor(file, Cr, V, v3);

                //table de huffman de DC pour Y
                struct huff_table* ht_dc_Y = huffman_table_build(htables_nb_symb_per_lengths[DC][Y], htables_symbols[DC][Y], htables_nb_symbols[DC][Y]);
                me_jpeg_set_huffman_table(file, DC, Y, ht_dc_Y);
                
                //table de huffman de DC pour Cb
                struct huff_table* ht_dc_Cb = huffman_table_build(htables_nb_symb_per_lengths[DC][Cb], htables_symbols[DC][Cb], htables_nb_symbols[DC][Cb]);
                me_jpeg_set_huffman_table(file, DC, Cb, ht_dc_Cb);

                //table de huffman de DC pour Cr
                struct huff_table* ht_dc_Cr = huffman_table_build(htables_nb_symb_per_lengths[DC][Cr], htables_symbols[DC][Cr], htables_nb_symbols[DC][Cr]);
                me_jpeg_set_huffman_table(file, DC, Cr, ht_dc_Cr);
            
                //table de huffman de AC pour Y
                struct huff_table* ht_ac_Y = huffman_table_build(htables_nb_symb_per_lengths[AC][Y], htables_symbols[AC][Y], htables_nb_symbols[AC][Y]);
                me_jpeg_set_huffman_table(file, AC, Y, ht_ac_Y);

                //table de huffman de AC pour Cb
                struct huff_table* ht_ac_Cb = huffman_table_build(htables_nb_symb_per_lengths[AC][Cb], htables_symbols[AC][Cb], htables_nb_symbols[AC][Cb]);
                me_jpeg_set_huffman_table(file, AC, Cb, ht_ac_Cb);

                //table de huffman de AC pour Cr
                struct huff_table* ht_ac_Cr = huffman_table_build(htables_nb_symb_per_lengths[AC][Cr], htables_symbols[AC][Cr], htables_nb_symbols[AC][Cr]);
                me_jpeg_set_huffman_table(file, AC, Cr, ht_ac_Cr);

                //table de quantification Y
                me_jpeg_set_quantization_table(file, Y, quantification_table_Y);
                //table de quantification Cb
                me_jpeg_set_quantization_table(file, Cb, quantification_table_CbCr);
                //table de quantificationCr
                me_jpeg_set_quantization_table(file, Cr, quantification_table_CbCr);

                me_jpeg_write_header(file);

                //On récupère le bitsream de notre de structure.
                struct my_bitstream* brute_stream = me_jpeg_get_bitstream(file);
                
                //debut traitement des mcu

                int32_t* spatial_bloc_Y = NULL;
                int32_t* frequential_bloc_Y = (int32_t*) calloc(64, sizeof(int32_t));

                int32_t* spatial_bloc_Cb = NULL;
                int32_t* frequential_bloc_Cb = (int32_t*) calloc(64, sizeof(int32_t));

                int32_t* spatial_bloc_Cr = NULL;
                int32_t* frequential_bloc_Cr = (int32_t*) calloc(64, sizeof(int32_t));

                int32_t* mcu_Y = (int32_t*) calloc(img->current_mcu->total_size, sizeof(int32_t));
                
                int32_t* mcu_Cb = (int32_t*) calloc(img->current_mcu->total_size, sizeof(int32_t));
                int32_t* mcu_Cb_down = NULL;

                int32_t* mcu_Cr = (int32_t*) calloc(img->current_mcu->total_size, sizeof(int32_t));
                int32_t* mcu_Cr_down = NULL;

                int32_t last_dc_Y = 0;
                int32_t last_dc_Cb = 0;
                int32_t last_dc_Cr = 0;

                int32_t * auxiliaire = NULL;
                next_matrix(img);
                bool conti = true;

                while(conti){       
                        conti = new_mcu(img);
                        
                        //On découpe en RGB puis on traduit en YCbCr dans la même fonction.
                        rgb_to_YcbCr(img_get_current_mcu(img), mcu_Y, mcu_Cb, mcu_Cr, img->current_mcu->total_size);  
       
                        mcu_Cb_down = downsampling(mcu_Cb, img->current_mcu->total_size, h1, v1, h2, v2);
                        free(mcu_Cb);

                        mcu_Cr_down = downsampling(mcu_Cr, img->current_mcu->total_size, h1, v1, h3, v3);
                        free(mcu_Cr);
                        auxiliaire = mcu_Y;

                        mcu_Y = reorder(auxiliaire, img->current_mcu->total_size, h1);
                        free(auxiliaire);

                        mcu_Cb = reorder(mcu_Cb_down, img->current_mcu->total_size*h2/h1*v2/v1, h2);
                        free(mcu_Cb_down);

                        mcu_Cr = reorder(mcu_Cr_down, img->current_mcu->total_size*h3/h1*v3/v1, h3);
                        free(mcu_Cr_down);
                        
                        //On retranche -128 à chaque composante. 
                        for(uint16_t i = 0; i < img->current_mcu->total_size; i++ ){
                                mcu_Y[i] -= 128;
                        }
                        
                        for(uint16_t i = 0; i < img->current_mcu->total_size*h2/h1*v2/v1; i++){                             
                                mcu_Cb[i] -= 128;
                        }

                        for(uint16_t i = 0; i < img->current_mcu->total_size*h3/h1*v3/v1; i++){
                                mcu_Cr[i] -= 128;
                        }
                       
                        for(uint8_t i = 1; i <= h1*v1; i++){              
                                spatial_bloc_Y = tronc_vector(mcu_Y, (i-1)*64, i*64);
                            
                                dct_function(spatial_bloc_Y, frequential_bloc_Y);
                              
                                auxiliaire = reorganization_zigzag(frequential_bloc_Y, quantification_table_Y);
                                free(frequential_bloc_Y);
                                frequential_bloc_Y = auxiliaire;

                                encode_dc(brute_stream,frequential_bloc_Y[0]-last_dc_Y,ht_dc_Y);
                                last_dc_Y = frequential_bloc_Y[0];

                                encode_ac(brute_stream,frequential_bloc_Y,ht_ac_Y);

                                free(spatial_bloc_Y);
                        }
                        
                        for(uint8_t i = 1; i <= h2*v2; i++){
                                spatial_bloc_Cb = tronc_vector(mcu_Cb, (i-1)*64, i*64);
                                
                                dct_function(spatial_bloc_Cb, frequential_bloc_Cb);
                                
                                auxiliaire = reorganization_zigzag(frequential_bloc_Cb, quantification_table_CbCr);
                                free(frequential_bloc_Cb);
                                frequential_bloc_Cb = auxiliaire;

                                encode_dc(brute_stream,frequential_bloc_Cb[0]-last_dc_Cb,ht_dc_Cb);
                                last_dc_Cb = frequential_bloc_Cb[0];

                                encode_ac(brute_stream,frequential_bloc_Cb,ht_ac_Cb);

                                free(spatial_bloc_Cb);
                        }
                        
                        for(uint8_t i = 1; i <= h3*v3; i++){
                                spatial_bloc_Cr = tronc_vector(mcu_Cr, (i-1)*64, i*64);
                               
                                dct_function(spatial_bloc_Cr, frequential_bloc_Cr);

                                dct_2(spatial_bloc_Cr, frequential_bloc_Cr);
                                auxiliaire = reorganization_zigzag(frequential_bloc_Cr, quantification_table_CbCr);
                                free(frequential_bloc_Cr);
                                frequential_bloc_Cr = auxiliaire;

                                encode_dc(brute_stream,frequential_bloc_Cr[0]-last_dc_Cr,ht_dc_Cr);
                                last_dc_Cr = frequential_bloc_Cr[0];

                                encode_ac(brute_stream,frequential_bloc_Cr,ht_ac_Cr);
                                
                                free(spatial_bloc_Cr);
                        }
                        
                free(mcu_Cr);
                mcu_Cr = (int32_t*) calloc(img->current_mcu->total_size, sizeof(int32_t));
                free(mcu_Cb);
                mcu_Cb = (int32_t*) calloc(img->current_mcu->total_size, sizeof(int32_t));
                }
        free(mcu_Y);
        free(mcu_Cb);
        free(mcu_Cr);
        free(frequential_bloc_Y);
        free(frequential_bloc_Cr);
        free(frequential_bloc_Cb);
        huffman_table_destroy(ht_dc_Y);
        huffman_table_destroy(ht_dc_Cb);
        huffman_table_destroy(ht_dc_Cr);
        huffman_table_destroy(ht_ac_Y);
        huffman_table_destroy(ht_ac_Cb);
        huffman_table_destroy(ht_ac_Cr);
        }
        me_jpeg_write_footer(file);
        me_jpeg_destroy(file);
        free_image(img);
       
}

/*
    Fonction qui ne fait qu'afficher le menue d'aide sur la sortie standart.
*/
void get_help(const char *filename){
    fprintf(stdout, "Usage: %s [OPTION] FILENAME\n", filename);
    fprintf(stdout, "Performs JPEG compression on PPM/PGM image.\n\n");
    fprintf(stdout, "Optional arguments:\n");
    fprintf(stdout, "     --outfile=NAME                         Sets output file to NAME.\n");
    fprintf(stdout, "                                            Default being FILENAME.jpg\n");
    fprintf(stdout, "     --sample=h1xv1,h2xv2,h3xv3             Sets respective subsampling\n");
    fprintf(stdout, "                                            factors for luminance and\n");
    fprintf(stdout, "                                            chrominance.\n");
    fprintf(stdout, "                                            Default is 1x1,1x1,1x1.\n");
    fprintf(stdout, "     --mode={p,b}                           Sets JPEG compression type\n");
    fprintf(stdout, "                                            p for progressive, b for baseline.\n");
    fprintf(stdout, "                                            Default is baseline.\n");
    fprintf(stdout, "                                            when specified.\n");
    fprintf(stdout, "     --dct={ii, loeffler}                   Uses adequate DCT algorithm.\n");
    fprintf(stdout, "                                            (DCT-II or Loeffler algorithm) .\n");
    fprintf(stdout, "                                            Default is DCT-II.\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "WARNING: Last argument MUST always be FILENAME.\n");


}
/*
    Cette fonction vérifie les paramètres de la ligne de commande.
*/
int check_factors(uint8_t h1, uint8_t v1, uint8_t h2, uint8_t v2, uint8_t h3, uint8_t v3, const char *file){
    if (h1 < 1 || h1 > 4 || h2 < 1 || h2 > 4 || h3 < 1 || h3 > 4){
        fprintf(stderr, "%s: Invalid subsampling factors. Values must be positive integers between 1 and 4.\n", file);
        return EXIT_FAILURE;
    }
    else if (v1 < 1 || v1 > 4 || v2 < 1 || v2 > 4 || v3 < 1 || v3 > 4){
        fprintf(stderr, "%s: Invalid subsampling factors. Values must be positive integers between 1 and 4.\n", file);
        return EXIT_FAILURE;
    }
    else if (v1*h1 + v2*h2 + v3*h3 > 10){
        fprintf(stderr, "%s: Invalid subsampling factors. Sum of product of values for luminance and chrominance must be less or equal to 10.\n", file);
        return EXIT_FAILURE;
    }
    else if (h1 % h2 != 0 || h1 % h3 != 0 || v1 % v2 != 0 || v1 % v3 != 0){
        fprintf(stderr, "%s: Invalid subsampling factors. Values for chrominance must be a divisor of value for luminance.\n", file);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}



int main(int argc, const char *argv[]){
    uint8_t h1 = 1;
    uint8_t v1 = 1;
    uint8_t h2 = 1;
    uint8_t v2 = 1;
    uint8_t h3 = 1;
    uint8_t v3 = 1;
    uint8_t p = 0;
    uint8_t dct_type = 0;
    char outfile[100] = {'\0'};
    char filename[100];
    FILE *file;

    if (argc == 1 || argc > 7){
        fprintf(stderr, "Invalid number of arguments\nTry \'%s --help\' for more information.\n", argv[0]);
        return EXIT_FAILURE;
    }
    else if (argc == 2){
        if (strcmp(argv[1], "--help") == 0){
            get_help(argv[0]);
            return EXIT_SUCCESS;
        }
    }
    else{
        for (uint8_t i = 1; i < argc - 1; i++){
            if (strncmp(argv[i], "--outfile=", 10) == 0){
                memcpy(outfile, &argv[i][10], 100);

                if (strlen(outfile) == 0){
                    fprintf(stderr, "Invalid output file name\nTry \'%s --help\' for more information.\n", argv[0]);
                    return EXIT_FAILURE;
                }
            }
            else if (strncmp(argv[i], "--sample=", 9) == 0){
                if (6 == sscanf(argv[i], "%*[^0123456789]%hhd%*[^0123456789]%hhd%*[^0123456789]%hhd%*[^0123456789]%hhd%*[^0123456789]%hhd%*[^0123456789]%hhd",
                &h1,
                &v1,
                &h2,
                &v2,
                &h3,
                &v3)){
                    if (check_factors(h1, v1, h2, v2, h3, v3, argv[0]) == EXIT_FAILURE){
                        return EXIT_FAILURE;
                    }
                }
                else{
                    fprintf(stderr, "Invalid subsampling factors\nTry \'%s --help\' for more information.\n", argv[0]);
                    return EXIT_FAILURE;
                }
            }
            else if (strncmp(argv[i], "--mode=p", 8) == 0){
                p = 1;
            }
            else if (strncmp(argv[i], "--mode=b", 8) == 0){
                p = 0;
            }
            else if(strncmp(argv[i], "--dct=loeffler", 14) == 0){
                dct_type = 1;
            }
            else if(strncmp(argv[i], "--dct=ii", 8) == 0){
                dct_type = 0;
            }
            else{
                fprintf(stderr, "Invalid arguments\nTry \'%s --help\' for more information.\n", argv[0]);
                return EXIT_FAILURE;
            }

        }

    }
    strcpy(filename, argv[argc - 1]);
    int32_t len = strlen(filename);
    file = fopen(filename, "r");
    if(file != NULL){
        fclose(file);
    }
    else{
        fprintf(stderr, "%s: cannot access \'%s\': No such file or directory.\n", argv[0], filename);
        return EXIT_FAILURE;
    }

    const char *extension = &filename[len - 4];
    if (strcmp(extension, ".ppm") != 0 && strcmp(extension, ".pgm") != 0){
        fprintf(stderr, "%s: Invalid file extension. Supported extensions are .ppm and .pgm.\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (strlen(outfile) == 0){
        strncpy(outfile, filename, len-4);
        strcat(outfile, ".jpg");
    }
    if (p == 0){
        ppm2jpeg(h1, v1, h2, v2, h3, v3, filename, outfile, dct_type);
    }
    else{
        progressive(h1, v1, h2, v2, h3, v3, filename, outfile);
    }
    printf("File %s generated successfully.\n", outfile);
    return EXIT_SUCCESS;

}

