#include <stdlib.h>
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
/*

struct progressive_stream{
    struct my_bitstream* stream_DC;
    struct my_bitstream* stream_ACY1_5;
    struct my_bitstream* stream_ACY6_63;
    struct my_bitstream* stream_ACCb1_5;
    struct my_bitstream* stream_ACCb6_63;
    struct my_bitstream* stream_ACCr1_5;
    struct my_bitstream* stream_ACCr6_63;
};

struct progressive_stream *create_progressive_stream(uint8_t type, struct me_jpeg *file, FILE *ACY1_5, FILE *ACY6_63, FILE *ACCb1_5, FILE *ACCb6_63, FILE *ACCr1_5, FILE *ACCr6_63)
{
    struct progressive_stream *streams = malloc(sizeof(struct progressive_stream));
    if (type == 1)
    {
        struct my_bitstream *stream_DC = me_jpeg_get_bitstream(file);
        struct my_bitstream *stream_ACY1_5 = my_bitstream_create(ACY1_5);
        progressive_SOS(ACY1_5, 1, 63);
        struct my_bitstream *stream_ACY6_63 = my_bitstream_create(ACY6_63);
        progressive_SOS(ACY6_63, 6, 63);
    }
    else
    {
        struct my_bitstream *stream_DC = me_jpeg_get_bitstream(file);
        struct my_bitstream *stream_ACY1_5 = my_bitstream_create(&ACY1_5);
        struct my_bitstream *stream_ACY6_63 = my_bitstream_create(&ACY6_63);
        struct my_bitstream *stream_ACCb1_5 = my_bitstream_create(&ACCb1_5);
        struct my_bitstream *stream_ACCb6_63 = my_bitstream_create(&ACCb6_63);
        struct my_bitstream *stream_ACCr1_5 = my_bitstream_create(&ACCr1_5);
        struct my_bitstream *stream_ACCr6_63 = my_bitstream_create(&ACCr6_63);
    }
    return streams;
}
*/
void progressive(uint8_t h1, uint8_t v1, uint8_t h2, uint8_t v2, uint8_t h3, uint8_t v3, char *filename, char *outfile)
{
    struct image *img = convert_file(filename, h1, v1);
    // On construit la structure me_jpeg/
    struct me_jpeg *file = me_jpeg_create();

    me_jpeg_set_ppm_filename(file, filename);
    me_jpeg_set_me_jpeg_filename(file, outfile);
    me_jpeg_set_image_height(file, img->row);
    me_jpeg_set_image_width(file, img->col);

    if (img->type == 5)
    { // rendre genrale pour couleur
        FILE *ACY1_5;
        // Coefficient AC de Y, Selection spectrale 1 à 5
        ACY1_5 = fopen("./progressive/ACY1_5.bin", "wb+");
        // Coefficient AC de Y, Selection spectrale 6 à 63
        //ACY6_63 = fopen("ACY6_63.bin", "wb+");
        me_jpeg_set_nb_components(file, 1);

        // rendre generale pour couleur
        me_jpeg_set_sampling_factor(file, 0, 0, 1);
        me_jpeg_set_sampling_factor(file, 0, 1, 1);

        // rendre generale pour couleur
        struct huff_table *ht_dc = huffman_table_build(htables_nb_symb_per_lengths[DC][0], htables_symbols[DC][0], htables_nb_symbols[DC][0]);
        me_jpeg_set_huffman_table(file, 0, 0, ht_dc);

        // rendre generale pour couleur
        struct huff_table *ht_ac = huffman_table_build(htables_nb_symb_per_lengths[AC][0], htables_symbols[AC][0], htables_nb_symbols[AC][0]);
        me_jpeg_set_huffman_table(file, 1, 0, ht_ac);

        me_jpeg_set_quantization_table(file, 0, quantification_table_Y);

        progressive_header(file);
        progressive_SOS(ACY1_5, 1, 63, 0, 0);
        struct my_bitstream *stream_DC = me_jpeg_get_bitstream(file);
        struct my_bitstream *stream_ACY1_5 = my_bitstream_create(ACY1_5);
        //struct my_bitstream *stream_ACY6_63 = my_bitstream_create(ACY6_63);
        // printf("Adresse = %p\n", stream_ACY1_5);

        // debut traitement des mcu
        int32_t *spatial_bloc = NULL;
        int32_t *frequential_bloc = NULL;
        int32_t *auxiliaire = NULL;
        int32_t last_dc = 0;
        frequential_bloc = (int32_t *)calloc(64, sizeof(int32_t));

        next_matrix(img);
        bool conti = true;
        while (conti)
        {
            conti = new_mcu(img);
            spatial_bloc = img_get_current_mcu(img);
            for (int i = 0; i < 64; i++)
            {
                spatial_bloc[i] -= 128;
            }

            dct_2(spatial_bloc, frequential_bloc);

            auxiliaire = reorganization_zigzag(frequential_bloc, quantification_table_Y);
            free(frequential_bloc);
            frequential_bloc = auxiliaire;
            //quantification(frequential_bloc, quantification_table_Y);

            encode_dc(stream_DC ,frequential_bloc[0]-last_dc,ht_dc);
            last_dc = frequential_bloc[0];
            encode_ac(stream_ACY1_5, frequential_bloc, ht_ac);
            // encode_AC_prog(stream_ACY1_5, stream_ACY6_63 ,frequential_bloc,0, EOBRUN);
        }

        free(frequential_bloc);
        huffman_table_destroy(ht_dc);
        huffman_table_destroy(ht_ac);
        my_bitstream_destroy(stream_DC);
        my_bitstream_destroy(stream_ACY1_5);
        footer_progressive(ACY1_5);
        concatenate_file(file, ACY1_5);
        

        me_jpeg_destroy(file);
        
    }

    else
    {
        FILE *ACCb1_63, *ACCr1_63, *ACY1_63, *DCCb, *DCCr;
        // SOS pour Coefficient AC de Cb, Selection spectrale 1 à 5
        ACCb1_63 = fopen("./progressive/ACCb1_63.bin", "wb+");
        // Coefficient AC de Cr, Selection spectrale 1 à 5
        ACCr1_63 = fopen("./progressive/ACCr1_63.bin", "wb+");
        // Coefficient AC de Y, Selection spectrale 1 à 5
        ACY1_63 = fopen("./progressive/ACY1_63.bin", "wb+");
        DCCb = fopen("./progressive/DCCb.bin", "wb+");
        DCCr = fopen("./progressive/DCCr.bin", "wb+");
        // Coefficient AC de Cb, Selection spectrale 6 à 63
        //ACCb6_63 = fopen("ACCb6_63.bin", "wb+");
        // Coefficient AC de Cr, Selection spectrale 6 à 63
        //ACCr6_63 = fopen("ACCr6_63.bin", "wb+");
        // Coefficient AC de Y, Selection spectrale 6 à 63
        //ACY6_63 = fopen("ACY6_63.bin", "wb+");
        me_jpeg_set_nb_components(file, 3);

        // Y
        me_jpeg_set_sampling_factor(file, 0, 0, h1);
        me_jpeg_set_sampling_factor(file, 0, 1, v1);
        // Cb
        me_jpeg_set_sampling_factor(file, 1, 0, h2);
        me_jpeg_set_sampling_factor(file, 1, 1, v2);
        // Cr
        me_jpeg_set_sampling_factor(file, 2, 0, h3);
        me_jpeg_set_sampling_factor(file, 2, 1, v3);

        // Y
        struct huff_table *ht_dc_0 = huffman_table_build(htables_nb_symb_per_lengths[DC][0], htables_symbols[DC][0], htables_nb_symbols[DC][0]);
        me_jpeg_set_huffman_table(file, 0, 0, ht_dc_0);

        // Cb
        struct huff_table *ht_dc_1 = huffman_table_build(htables_nb_symb_per_lengths[DC][1], htables_symbols[DC][1], htables_nb_symbols[DC][1]);
        me_jpeg_set_huffman_table(file, 0, 1, ht_dc_1);

        // Cr
        struct huff_table *ht_dc_2 = huffman_table_build(htables_nb_symb_per_lengths[DC][2], htables_symbols[DC][2], htables_nb_symbols[DC][2]);
        me_jpeg_set_huffman_table(file, 0, 2, ht_dc_2);

        // Y
        struct huff_table *ht_ac_0 = huffman_table_build(htables_nb_symb_per_lengths[AC][0], htables_symbols[AC][0], htables_nb_symbols[AC][0]);
        me_jpeg_set_huffman_table(file, 1, 0, ht_ac_0);

        // Cb
        struct huff_table *ht_ac_1 = huffman_table_build(htables_nb_symb_per_lengths[AC][1], htables_symbols[AC][1], htables_nb_symbols[AC][1]);
        me_jpeg_set_huffman_table(file, 1, 1, ht_ac_1);

        // Cr
        struct huff_table *ht_ac_2 = huffman_table_build(htables_nb_symb_per_lengths[AC][2], htables_symbols[AC][2], htables_nb_symbols[AC][2]);
        me_jpeg_set_huffman_table(file, 1, 2, ht_ac_2);

        // Y
        me_jpeg_set_quantization_table(file, 0, quantification_table_Y);
        // Cb
        me_jpeg_set_quantization_table(file, 1, quantification_table_CbCr);
        // Cr
        me_jpeg_set_quantization_table(file, 2, quantification_table_CbCr);

        progressive_header(file);
        progressive_SOS(DCCb, 0, 0, 1, 0x11);
        progressive_SOS(DCCr, 0, 0, 2, 0x11);
        progressive_SOS(ACY1_63, 1, 63, 0,0x0);
        progressive_SOS(ACCb1_63, 1, 63, 1, 0x11);
        progressive_SOS(ACCr1_63, 1, 63, 2, 0x11);

        struct my_bitstream *stream_DC = me_jpeg_get_bitstream(file);
        struct my_bitstream *stream_ACY1_63 = my_bitstream_create(ACY1_63);
        struct my_bitstream *stream_ACCb1_63 = my_bitstream_create(ACCb1_63);
        struct my_bitstream *stream_ACCr1_63 = my_bitstream_create(ACCr1_63);
        struct my_bitstream *stream_DCCb = my_bitstream_create(DCCb);
        struct my_bitstream *stream_DCCr = my_bitstream_create(DCCr);

        // debut traitement des mcu
        int32_t *spatial_bloc_Y = NULL;
        int32_t *frequential_bloc_Y = (int32_t *)calloc(64, sizeof(int32_t));

        int32_t *spatial_bloc_Cb = NULL;
        int32_t *frequential_bloc_Cb = (int32_t *)calloc(64, sizeof(int32_t));

        int32_t *spatial_bloc_Cr = NULL;
        int32_t *frequential_bloc_Cr = (int32_t *)calloc(64, sizeof(int32_t));

        int32_t *mcu_Y = (int32_t *)calloc(img->current_mcu->total_size, sizeof(int32_t));

        int32_t *mcu_Cb = (int32_t *)calloc(img->current_mcu->total_size, sizeof(int32_t));
        int32_t *mcu_Cb_down_v = NULL;
        int32_t *mcu_Cb_down_h = NULL;

        int32_t *mcu_Cr = (int32_t *)calloc(img->current_mcu->total_size, sizeof(int32_t));
        int32_t *mcu_Cr_down_v = NULL;
        int32_t *mcu_Cr_down_h = NULL;

        int32_t last_dc_Y = 0;
        int32_t last_dc_Cb = 0;
        int32_t last_dc_Cr = 0;

        int32_t *auxiliaire = NULL;
        next_matrix(img);

        bool conti = true;
        while (conti)
        {
            conti = new_mcu(img);

            rgb_to_YcbCr(img_get_current_mcu(img), mcu_Y, mcu_Cb, mcu_Cr, img->current_mcu->total_size);

            if (h1 == h2)
            {
                mcu_Cb_down_h = mcu_Cb;
            }
            else
            {
                mcu_Cb_down_h = horizontal_downsampling(mcu_Cb, img->current_mcu->total_size, h1, h2);
                free(mcu_Cb);
            }
            if (v1 == v2)
            { 

                mcu_Cb_down_v = mcu_Cb_down_h;
            }
            else
            { 
                mcu_Cb_down_v = vertical_downsampling(mcu_Cb_down_h, img->current_mcu->total_size * h2 / h1, h2, v1, v2);
                free(mcu_Cb_down_h);
            }

            if (h1 == h3)
            { 
                mcu_Cr_down_h = mcu_Cr;
            }
            else
            { 

                mcu_Cr_down_h = horizontal_downsampling(mcu_Cr, img->current_mcu->total_size, h1, h3);
                free(mcu_Cr);
            }

            if (v1 == v3)
            {
                mcu_Cr_down_v = mcu_Cr_down_h;
            }
            else
            {
                mcu_Cr_down_v = vertical_downsampling(mcu_Cr_down_h, img->current_mcu->total_size * h3 / h1, h3, v1, v3);
                free(mcu_Cr_down_h);
            }
            //auxiliaire = mcu_Y;
            //mcu_Y = reorder(auxiliaire, img->current_mcu->total_size, h1, v1);
            //free(auxiliaire);
            mcu_Cb = reorder(mcu_Cb_down_v, img->current_mcu->total_size * h2 / h1 * v2 / v1, h2);
            free(mcu_Cb_down_v);
            mcu_Cr = reorder(mcu_Cr_down_v, img->current_mcu->total_size * h3 / h1 * v3 / v1, h3);
            free(mcu_Cr_down_v);
            /*
            for (uint16_t i = 0; i < img->current_mcu->total_size; i++)
            {
                mcu_Y[i] -= 128;
            }*/

            for (uint16_t i = 0; i < img->current_mcu->total_size * h2 / h1 * v2 / v1; i++)
            {
                mcu_Cb[i] -= 128;
            }
            for (uint16_t i = 0; i < img->current_mcu->total_size * h3 / h1 * v3 / v1; i++)
            {
                mcu_Cr[i] -= 128;
            }
            /*
            for (uint8_t i = 1; i <= h1 * v1; i++)
            {
                spatial_bloc_Y = tronc_vector(mcu_Y, (i - 1) * 64, i * 64);
                dct_2(spatial_bloc_Y, frequential_bloc_Y);

                auxiliaire = reorganization_zigzag(frequential_bloc_Y, quantification_table_Y);

                free(frequential_bloc_Y);
                frequential_bloc_Y = auxiliaire;

                //quantification(frequential_bloc_Y, quantification_table_Y);

                encode_dc(stream_DC, frequential_bloc_Y[0] - last_dc_Y, 0);
                
                


                last_dc_Y = frequential_bloc_Y[0];
                encode_ac(stream_ACY1_63, frequential_bloc_Y, 0);


                free(spatial_bloc_Y);
            }*/
            for (uint8_t i = 1; i <= h2 * v2; i++)
            {
                spatial_bloc_Cb = tronc_vector(mcu_Cb, (i - 1) * 64, i * 64);
                dct_2(spatial_bloc_Cb, frequential_bloc_Cb);
                auxiliaire = reorganization_zigzag(frequential_bloc_Cb, quantification_table_CbCr);
                free(frequential_bloc_Cb);
                frequential_bloc_Cb = auxiliaire;
                //quantification(frequential_bloc_Cb, quantification_table_CbCr);
                encode_dc(stream_DCCb, frequential_bloc_Cb[0] - last_dc_Cb, ht_dc_1);
                last_dc_Cb = frequential_bloc_Cb[0];
                encode_ac(stream_ACCb1_63, frequential_bloc_Cb, ht_ac_1);
                free(spatial_bloc_Cb);
            }
            for (uint8_t i = 1; i <= h3 * v3; i++)
            {
                spatial_bloc_Cr = tronc_vector(mcu_Cr, (i - 1) * 64, i * 64);
                dct_2(spatial_bloc_Cr, frequential_bloc_Cr);
                auxiliaire = reorganization_zigzag(frequential_bloc_Cr, quantification_table_CbCr);
                free(frequential_bloc_Cr);
                frequential_bloc_Cr = auxiliaire;
                //quantification(frequential_bloc_Cr, quantification_table_CbCr);
                encode_dc(stream_DCCr, frequential_bloc_Cr[0] - last_dc_Cr, ht_dc_2);
                last_dc_Cr = frequential_bloc_Cr[0];
                encode_ac(stream_ACCr1_63, frequential_bloc_Cr, ht_ac_2);
                free(spatial_bloc_Cr);
            }
            free(mcu_Cr);
            mcu_Cr = (int32_t *)calloc(img->current_mcu->total_size, sizeof(int32_t));
            free(mcu_Cb);
            mcu_Cb = (int32_t *)calloc(img->current_mcu->total_size, sizeof(int32_t));
        }
        struct image *img2 = convert_file(filename, 1, 1);
        next_matrix(img2);

        conti = true;
        while (conti)
        {
            conti = new_mcu(img2);

            rgb_to_YcbCr(img_get_current_mcu(img2), mcu_Y, mcu_Cb, mcu_Cr, img->current_mcu->total_size);

            
            for (uint16_t i = 0; i < img->current_mcu->total_size; i++)
            {
                mcu_Y[i] -= 128;
            }

            
            
            
            spatial_bloc_Y = tronc_vector(mcu_Y, 0, 64);
            dct_2(spatial_bloc_Y, frequential_bloc_Y);

            auxiliaire = reorganization_zigzag(frequential_bloc_Y, quantification_table_Y);

            free(frequential_bloc_Y);
            frequential_bloc_Y = auxiliaire;

            //quantification(frequential_bloc_Y, quantification_table_Y);

            encode_dc(stream_DC, frequential_bloc_Y[0] - last_dc_Y, ht_dc_0);
            
            


            last_dc_Y = frequential_bloc_Y[0];
            encode_ac(stream_ACY1_63, frequential_bloc_Y, ht_ac_0);


            free(spatial_bloc_Y);
            
            
        }


        /*
        struct image *imgcr = convert_file(filename, h2/h1, v2/v1);
        next_matrix(imgcr);
        conti = true;
        while (conti)
        {
            conti = new_mcu(imgcr);

            rgb_to_YcbCr(img_get_current_mcu(imgcr), mcu_Y, mcu_Cb, mcu_Cr, img->current_mcu->total_size);

            if (h1 == h2)
            {
                mcu_Cb_down_h = mcu_Cb;
            }
            else
            {
                mcu_Cb_down_h = horizontal_downsampling(mcu_Cb, img->current_mcu->total_size, h1/h2, h2);
                free(mcu_Cb);
            }
            if (v1 == v2)
            { 

                mcu_Cb_down_v = mcu_Cb_down_h;
            }
            else
            { 
                mcu_Cb_down_v = vertical_downsampling(mcu_Cb_down_h, img->current_mcu->total_size * h2 / h1, h2, v1, v2);
                free(mcu_Cb_down_h);
            }

            
            mcu_Cb = mcu_Cb_down_v;
            free(mcu_Cb_down_v);
            

            for (uint16_t i = 0; i < 64; i++)
            {
                mcu_Cb[i] -= 128;
            }
            

            
            spatial_bloc_Cb = tronc_vector(mcu_Cb, 0, 64);
            dct_2(spatial_bloc_Cb, frequential_bloc_Cb);
            auxiliaire = reorganization_zigzag(frequential_bloc_Cb, quantification_table_CbCr);
            free(frequential_bloc_Cb);
            frequential_bloc_Cb = auxiliaire;
            //quantification(frequential_bloc_Cb, quantification_table_CbCr);
            encode_dc(stream_DCCb, frequential_bloc_Cb[0] - last_dc_Cb, 1);
            last_dc_Cb = frequential_bloc_Cb[0];
            encode_ac(stream_ACCb1_63, frequential_bloc_Cb, 1);
            free(spatial_bloc_Cb);
        
            
            free(mcu_Cb);
            mcu_Cb = (int32_t *)calloc(img->current_mcu->total_size, sizeof(int32_t));
        }
        */



        free(mcu_Y);
        free(mcu_Cb);
        free(mcu_Cr);
        free(frequential_bloc_Y);
        free(frequential_bloc_Cr);
        free(frequential_bloc_Cb);
        huffman_table_destroy(ht_dc_0);
        huffman_table_destroy(ht_dc_1);
        huffman_table_destroy(ht_dc_2);
        huffman_table_destroy(ht_ac_0);
        huffman_table_destroy(ht_ac_1);
        huffman_table_destroy(ht_ac_2);
        my_bitstream_destroy(stream_DC);
        my_bitstream_destroy(stream_DCCb);
        my_bitstream_destroy(stream_DCCr);
        my_bitstream_destroy(stream_ACY1_63);
        my_bitstream_destroy(stream_ACCb1_63);
        my_bitstream_destroy(stream_ACCr1_63);
        footer_progressive(ACCr1_63);
        concatenate_file(file, DCCb);
        concatenate_file(file, DCCr);
        concatenate_file(file, ACY1_63);
        concatenate_file(file, ACCb1_63);
        concatenate_file(file, ACCr1_63);
        me_jpeg_destroy(file);
        free_image(img2);
    }
    free_image(img);
}

/*
int main(int argc, const char *argv[])
{
    printf(" %s\n", argv[1]);
    progressive(1, 1, 1, 1, 1, 1, "images/biiiiiig.ppm", "test.jpg");
}
*/