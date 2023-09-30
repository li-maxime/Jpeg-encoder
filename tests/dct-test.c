#include <stdio.h>
#include <stdlib.h>
#include <dct.h>

int main()
{
      
        float  *cos_table = set_cos_table();
        int32_t *freq_bloc = calloc(64, sizeof(int32_t));
        int32_t *bloc = calloc(64, sizeof(int32_t));

        //compute_dct(space_bloc, freq_bloc, 8);
        int32_t space_bloc[64] = {
                140,144,147,140,140,155,179,175,
                144,152,140,147,140,148,167,179,
                152,155,136,167,163,162,152,172,
                168,145,156,160,152,155,136,160,
                162,148,156,148,140,136,147,162,
                147,167,140,155,155,140,136,162,
                136,156,123,167,162,144,140,147,
                148,155,136,155,152,147,147,136
        };   

        for(int y = 0; y < 64; y++){
                if(y%8 == 0){
                        printf("\n");
                }
                printf("%i ", space_bloc[y]);
        }

        printf("\n");

        compute_dct(space_bloc, freq_bloc, cos_table);

        for(int y = 0; y < 64; y++){
                if(y%8 == 0){
                        printf("\n");
                }
                printf("%i ", freq_bloc[y]);
        }

        printf("\n");

        dct_2(space_bloc, freq_bloc);
        for(int y = 0; y < 64; y++){
                if(y%8 == 0){
                        printf("\n");
                }
                printf("%04i ", freq_bloc[y]);
        }

        printf("\n");

        for(int y = 0; y < 64; y++){
                bloc[y] = space_bloc[y];
        }

        loeffler(bloc, freq_bloc);

        for(int y = 0; y < 64; y++){
                if(y%8 == 0){
                        printf("\n");
                }
                printf("%04i ", freq_bloc[y]);
                }
        printf("\n");

        int32_t space_bloc2[64] = 
        {   0xfd, 0xff, 0xfc, 0xff , 0xfe , 0xfe , 0xff , 0xff, 
                0xff , 0xff , 0xff , 0xfc , 0xff , 0xff , 0xfe , 0xfe, 
                0x18 , 0x7f , 0xcf , 0xff , 0xfc , 0xfc , 0xff , 0xfd, 
                0x01 , 0x00 , 0x05 , 0x55 , 0xcd , 0xfc , 0xfb , 0xff, 
                0x00 , 0x09 , 0x00 , 0x00 , 0x04 , 0x93 , 0xf5 , 0xfb, 
                0x00 , 0x04 , 0x04 , 0x00 , 0x03 , 0x00 , 0x98 , 0xfb, 
                0x05 , 0x01 , 0x06 , 0x01 , 0x01 , 0x00 , 0x15 , 0xc9, 
                0x00 , 0x02 , 0x00 , 0x00 , 0x04 , 0x00 , 0x01 , 0x38 };

        for(int y = 0; y < 64; y++){
                space_bloc2[y] -= 128;
                }
        loeffler(space_bloc2, freq_bloc);


        for(int y = 0; y < 64; y++){
                if(y%8 == 0){
                        printf("\n");
                }
                printf("%08x ", (freq_bloc[y]));
                }
                printf("\n");

        int32_t space_bloc3[64] = 
        {
        0xff, 0xff, 0xff, 0xff , 0xff , 0xff , 0xff , 0xff, 
                0xff, 0xff, 0xff, 0xff , 0xff , 0xff , 0xff , 0xff,  
                0xff, 0xff, 0xff, 0xff , 0xff , 0xff , 0xff , 0xff, 
                0xff, 0xff, 0xff, 0xff , 0xff , 0xff , 0xff , 0xff, 
                0xff, 0xff, 0xff, 0xff , 0xff , 0xff , 0xff , 0xff, 
                0xff, 0xff, 0xff, 0xff , 0xff , 0xff , 0xff , 0xff, 
                0xff, 0xff, 0xff, 0xff , 0xff , 0xff , 0xff , 0xff, 
                0xff, 0xff, 0xff, 0xff , 0xff , 0xff , 0xff , 0xff};

        for(int y = 0; y < 64; y++)
                {
                space_bloc3[y] -= 128;
                }
        loeffler(space_bloc3,freq_bloc);


        for(int y = 0; y < 64; y++)
                {
                if(y%8 == 0)
                {
                        printf("\n");
                }
                printf("%08x ", (freq_bloc[y]));
                }
                printf("\n");  
        free(cos_table);
        free(freq_bloc);
        free(bloc) ;
        return EXIT_SUCCESS;
}


 
/*

 

1210 -18 15 -9 23 -9 -14 -19
21 -34 26 -9 -11 11 14 7
-10 -24 -2 6 -18 3 -20 -1
-8 -5 14 -15 -8 -3 -3 8
-3 10 8 1 -11 18 18 15
4 -2 -18 8 8 -4 1 -7
9 1 -3 4 -1 -7 -1 -2
0 -8 -2 2 1 4 -6 0

int32_t space_bloc[64] = {
            140,144,147,140,140,155,179,175,
            144,152,140,147,140,148,167,179,
            147,155,136,167,163,162,152,172,
            140,145,156,160,152,155,136,160,
            140,148,156,148,140,136,147,162,
            155,167,140,155,155,140,136,162,
            179,156,123,167,162,144,140,147,
            175,155,136,155,152,147,147,136
    };
 
 

int32_t space_bloc[64] = {
            140,144,147,140,140,155,179,175,
            140,144,147,140,140,155,179,175,
            140,144,147,140,140,155,179,175,
            140,144,147,140,140,155,179,175,
            140,144,147,140,140,155,179,175,
            140,144,147,140,140,155,179,175,
            140,144,147,140,140,155,179,175,
            140,144,147,140,140,155,179,175
    }; 
*/
