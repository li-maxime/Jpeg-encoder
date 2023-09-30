#include <stdlib.h>
#include <dct.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <mcu.h>
#include <complex.h>

#define PI 3.14159265358979323846
#define SQRT2  1.41421356237
#define SQRT8  2.82842712475 
#define COS3 0.8314696123              //cos(3*pi/16)
#define SIN3 0.55557023302             //sin(3*pi/16)
#define ADD_CS3  1.38703985
#define COS1 0.9807852804              //cos(pi/16)
#define SIN1 0.19509032201              //sin(pi/16)
#define ADD_CS1  1.17587560241
#define COS6 0.38268343236              //cos(6*pi/16)
#define SIN6 0.92387953251              //sin(6*pi/16)
#define ADD_CS6 1.30656296487


//Coeff DCT II
#define C1 0.49039 //cos(PI/16)/2
#define C2 0.46194 //cos(2*PI/16)/2
#define C3 0.41573 //cos(3*PI/16)/2
#define C4 0.35355 //cos(4*PI/16)/2
#define C5 0.27779 //cos(5*PI/16)/2
#define C6 0.19134 //cos(6*PI/16)/2
#define C7 0.09755 //cos(7*PI/16)/2

static float ONE_OVER_SQRT2 = 1/sqrt(2);

/*
    Renvoie le coefficient c(xi)
*/
float coefficient(uint8_t xi){
    if(xi == 0){
        return ONE_OVER_SQRT2;
    }
    else{
        return 1.;
    }
}

/*
    Retourne un tableau des valeurs de cos pour les fonctions compute_dct, compute_dct_naive, compute_dct_less_op. 
    Tableau alloué dynamiquement, l'utilisateur doit libérer la mémoire lui même.
*/
float* set_cos_table(){
    float *cos_table = (float *)calloc(64, sizeof(float));
    for(uint8_t x = 0; x < 8; x++){
        for(uint8_t i = 0; i < 8; i++){
            cos_table[x+i*8] = cosf( ((2.* (float)x +1.)* ((float)i) * PI) / (float)(2*8));
        }
    }
    return cos_table;
}

/*
    Prend une matrice freq_bloc puis la remplie avec les coefficents de la dct de la matrice space_bloc.
    Version de base qui sert de référence.
*/
void compute_dct_naive(int32_t *space_bloc, int32_t* freq_bloc){
    float *cos_value = set_cos_table();
    float sum;
    for(uint8_t i = 0; i < 8; i++){
        for(uint8_t j = 0; j < 8; j++){
            sum = 0;
            for(uint8_t x = 0; x < 8; x++){
                for(uint8_t y = 0; y < 8; y++){
                    sum += space_bloc[x*8+y]*cos_value[x+i*8]*cos_value[y+j*8];
                }
            }
            freq_bloc[8*i+j] = (int32_t)((2*sum*coefficient(i)*coefficient(j))/8); 
        }
    }
    free(cos_value);
}

/*
    Prend une matrice freq_bloc puis la remplie avec les coefficents de la dct de la matrice space_bloc.
    On pré-calcul les valeurs du cosinus à l'exterieur et on passe le tableau de valeurs à la fonction.
*/
void compute_dct(int32_t *space_bloc, int32_t* freq_bloc, float *cos_table){
    float sum;
    for(uint8_t i = 0; i < 8; i++){
        for(uint8_t j = 0; j < 8; j++){
            sum = 0;
            for(uint8_t x = 0; x < 8; x++){
                for(uint8_t y = 0; y < 8; y++){
                    sum += space_bloc[x*8+y]*cos_table[x+i*8]*cos_table[y+j*8];
                }
            }
            freq_bloc[8*i+j] = (int32_t)((2*sum*coefficient(i)*coefficient(j))/8.); 
        }
    }
}
/*
    Prend une matrice freq_bloc puis la remplie avec les coefficents de la dct de la matrice space_bloc.
    On pré-calcul les valeurs du cosinus à l'exterieur et on passe le tableau de valeurs à la fonction.
    On fait moins de multiplication en factorisant un des deux cosinus.
*/
void compute_dct_less_op(int32_t *space_bloc, int32_t* freq_bloc, float *cos_table)
{

    float sum[8];
    float mem_sum;
    for(uint8_t i = 0; i < 8; i++){
        for(uint8_t j = 0; j < 8; j++){
            mem_sum = 0;
            for(uint8_t x = 0; x < 8; x++){
                sum[x] = 0;
                for(uint8_t y = 0; y < 8; y++){
                    sum[x] += space_bloc[x*8+y]*cos_table[y+j*8];
                }
                mem_sum += sum[x] *cos_table[x+i*8];
            }
            freq_bloc[8*i+j] = (int32_t)((2.*mem_sum*coefficient(i)*coefficient(j))/8.); 
        }
    }
}

/*
    Renvoie un tableau des coefficients nécessaire à la méthode de calcul dct-II.
    C_i = cos_table[i]
*/
float* set_coeff_dct2()
{
    float c = sqrt(0.25); //define plus tard
    float *cos_table = (float *)calloc(8, sizeof(float));
    for(uint8_t i = 0; i < 8; i++){
        cos_table[i] = c*cosf(((float)i) * PI / 16.);  
    }
    return cos_table;
}

/*
    Prend une matrice freq_bloc puis la remplie avec les coefficents de la dct de la matrice space_bloc.
    On pré-calcul les valeurs du cosinus à l'exterieur et on passe le tableau de valeurs à la fonction.
    On fait moins d'opération avec la méthode de calcul dct-II.
    Formule et matrice trouvées sur la page wiki de la dct.
*/
void dct_2(int32_t *space_bloc, int32_t* freq_bloc)
{
    int32_t auxiliaire[64];
    for(uint8_t i = 0; i < 8; i++){

        auxiliaire[8*i+0] = C4*(space_bloc[8*i+0]+space_bloc[8*i+7]) + C4*(space_bloc[8*i+1]+space_bloc[8*i+6]) + C4*(space_bloc[8*i+2]+space_bloc[8*i+5]) + C4*(space_bloc[8*i+3]+space_bloc[8*i+4]);
        auxiliaire[8*i+2] = C2*(space_bloc[8*i+0]+space_bloc[8*i+7]) + C6*(space_bloc[8*i+1]+space_bloc[8*i+6]) - C6*(space_bloc[8*i+2]+space_bloc[8*i+5]) - C2*(space_bloc[8*i+3]+space_bloc[8*i+4]);
        auxiliaire[8*i+4] = C4*(space_bloc[8*i+0]+space_bloc[8*i+7]) - C4*(space_bloc[8*i+1]+space_bloc[8*i+6]) - C4*(space_bloc[8*i+2]+space_bloc[8*i+5]) + C4*(space_bloc[8*i+3]+space_bloc[8*i+4]);
        auxiliaire[8*i+6] = C6*(space_bloc[8*i+0]+space_bloc[8*i+7]) - C2*(space_bloc[8*i+1]+space_bloc[8*i+6]) + C2*(space_bloc[8*i+2]+space_bloc[8*i+5]) - C6*(space_bloc[8*i+3]+space_bloc[8*i+4]);
        
        auxiliaire[8*i+1] = C1*(space_bloc[8*i+0]-space_bloc[8*i+7]) + C3*(space_bloc[8*i+1]-space_bloc[8*i+6]) + C5*(space_bloc[8*i+2]-space_bloc[8*i+5]) + C7*(space_bloc[8*i+3]-space_bloc[8*i+4]);
        auxiliaire[8*i+3] = C3*(space_bloc[8*i+0]-space_bloc[8*i+7]) - C7*(space_bloc[8*i+1]-space_bloc[8*i+6]) - C1*(space_bloc[8*i+2]-space_bloc[8*i+5]) - C5*(space_bloc[8*i+3]-space_bloc[8*i+4]);
        auxiliaire[8*i+5] = C5*(space_bloc[8*i+0]-space_bloc[8*i+7]) - C1*(space_bloc[8*i+1]-space_bloc[8*i+6]) + C7*(space_bloc[8*i+2]-space_bloc[8*i+5]) + C3*(space_bloc[8*i+3]-space_bloc[8*i+4]);
        auxiliaire[8*i+7] = C7*(space_bloc[8*i+0]-space_bloc[8*i+7]) - C5*(space_bloc[8*i+1]-space_bloc[8*i+6]) + C3*(space_bloc[8*i+2]-space_bloc[8*i+5]) - C1*(space_bloc[8*i+3]-space_bloc[8*i+4]);
   
   
    }
    
    for(uint8_t i = 0; i < 8; i++){
        freq_bloc[8*0+i] = C4*(auxiliaire[0*8+i]+auxiliaire[7*8+i]) + C4*(auxiliaire[1*8+i]+auxiliaire[6*8+i]) + C4*(auxiliaire[2*8+i]+auxiliaire[5*8+i]) + C4*(auxiliaire[3*8+i]+auxiliaire[4*8+i]);
        freq_bloc[8*2+i] = C2*(auxiliaire[0*8+i]+auxiliaire[7*8+i]) + C6*(auxiliaire[1*8+i]+auxiliaire[6*8+i]) - C6*(auxiliaire[2*8+i]+auxiliaire[5*8+i]) - C2*(auxiliaire[3*8+i]+auxiliaire[4*8+i]);
        freq_bloc[8*4+i] = C4*(auxiliaire[0*8+i]+auxiliaire[7*8+i]) - C4*(auxiliaire[1*8+i]+auxiliaire[6*8+i]) - C4*(auxiliaire[2*8+i]+auxiliaire[5*8+i]) + C4*(auxiliaire[3*8+i]+auxiliaire[4*8+i]);
        freq_bloc[8*6+i] = C6*(auxiliaire[0*8+i]+auxiliaire[7*8+i]) - C2*(auxiliaire[1*8+i]+auxiliaire[6*8+i]) + C2*(auxiliaire[2*8+i]+auxiliaire[5*8+i]) - C6*(auxiliaire[3*8+i]+auxiliaire[4*8+i]);
        
        freq_bloc[8*1+i] = C1*(auxiliaire[0*8+i]-auxiliaire[7*8+i]) + C3*(auxiliaire[1*8+i]-auxiliaire[6*8+i]) + C5*(auxiliaire[2*8+i]-auxiliaire[5*8+i]) + C7*(auxiliaire[3*8+i]-auxiliaire[4*8+i]);
        freq_bloc[8*3+i] = C3*(auxiliaire[0*8+i]-auxiliaire[7*8+i]) - C7*(auxiliaire[1*8+i]-auxiliaire[6*8+i]) - C1*(auxiliaire[2*8+i]-auxiliaire[5*8+i]) - C5*(auxiliaire[3*8+i]-auxiliaire[4*8+i]);
        freq_bloc[8*5+i] = C5*(auxiliaire[0*8+i]-auxiliaire[7*8+i]) - C1*(auxiliaire[1*8+i]-auxiliaire[6*8+i]) + C7*(auxiliaire[2*8+i]-auxiliaire[5*8+i]) + C3*(auxiliaire[3*8+i]-auxiliaire[4*8+i]);
        freq_bloc[8*7+i] = C7*(auxiliaire[0*8+i]-auxiliaire[7*8+i]) - C5*(auxiliaire[1*8+i]-auxiliaire[6*8+i]) + C3*(auxiliaire[2*8+i]-auxiliaire[5*8+i]) - C1*(auxiliaire[3*8+i]-auxiliaire[4*8+i]);
    } 
}
/*
    Prend une matrice freq_bloc puis la remplie avec les coefficents de la dct de la matrice bloc.
    Des macro sont définies au début de fichier (COS1, SIN1, COS2, SIN2, COS6, SIN6, SQRT2, SQRT8).
    On fait moins d'opération avec la méthode de calcul loeffler.
    Description de l'algo sur : https://www.ijaiem.org/volume3issue5/IJAIEM-2014-05-31-117.pdf (avec une erreur sur le schéma, oubli du sqrt2 pour le bloc C6).
*/
void loeffler(int32_t *bloc, int32_t *freq_bloc){
    /*
    Des matrices et variables auxiliaires pour résoudre le problème de dépendance entre les étapes.
    On prend des floatants pour minimiser les cast. 
    */
    int32_t space_bloc[64];
    int32_t auxiliaire[64];
    int32_t auxi_var1, auxi_var2, auxi_var3, auxi_var4, auxi_var5, auxi_var6, auxi_var7, auxi_var0;

    //DCT sur chaque ligne de la matrice spaciale.
    for(uint8_t i = 0; i < 8; i++){
        // Etape 1 
        auxi_var0 =  (bloc[8*i+0] + bloc[8*i+7]);
        auxi_var7 =  (bloc[8*i+0] - bloc[8*i+7]);

        auxi_var1 =  (bloc[8*i+1] + bloc[8*i+6]);
        auxi_var6 =  (bloc[8*i+1] - bloc[8*i+6]);

        auxi_var2 =  (bloc[8*i+2] + bloc[8*i+5]);
        auxi_var5 = (bloc[8*i+2] - bloc[8*i+5]);

        auxi_var3 =  (bloc[8*i+3] + bloc[8*i+4]);
        auxi_var4 =  (bloc[8*i+3] - bloc[8*i+4]);

        space_bloc[8*i+0] =  auxi_var0;
        space_bloc[8*i+1] =  auxi_var1;
        space_bloc[8*i+2] =  auxi_var2;
        space_bloc[8*i+3] =  auxi_var3;
        space_bloc[8*i+4] =  auxi_var4;
        space_bloc[8*i+5] =  auxi_var5;
        space_bloc[8*i+6] =  auxi_var6;
        space_bloc[8*i+7] =  auxi_var7;

        // Etape 2
        auxi_var0 = space_bloc[8*i+0] + space_bloc[8*i+3];
        auxi_var3 = space_bloc[8*i+0] - space_bloc[8*i+3];

        auxi_var1 = space_bloc[8*i+1] + space_bloc[8*i+2];
        auxi_var2 = space_bloc[8*i+1] - space_bloc[8*i+2];

        auxi_var4 = space_bloc[8*i+4]*COS3 + space_bloc[8*i+7]*SIN3;
        auxi_var7 = space_bloc[8*i+7]*COS3 - space_bloc[8*i+4]*SIN3;
        //auxi_var4 = (space_bloc[8*i+7]-space_bloc[8*i+4])*SIN3 + space_bloc[8*i+4]*ADD_CS3;
        //auxi_var7 = (-space_bloc[8*i+7]-space_bloc[8*i+4])*COS3 + space_bloc[8*i+4]*ADD_CS3;

        auxi_var5 = space_bloc[8*i+5]*COS1 + space_bloc[8*i+6]*SIN1;
        auxi_var6 = space_bloc[8*i+6]*COS1 - space_bloc[8*i+5]*SIN1;
        //auxi_var5 = (space_bloc[8*i+6]-space_bloc[8*i+5])*SIN1 + space_bloc[8*i+5]*ADD_CS1;
        //auxi_var6 = (-space_bloc[8*i+6]-space_bloc[8*i+5])*COS1 + space_bloc[8*i+5]*ADD_CS1;

        space_bloc[8*i+0] =  auxi_var0;
        space_bloc[8*i+1] =  auxi_var1;
        space_bloc[8*i+2] =  auxi_var2;
        space_bloc[8*i+3] =  auxi_var3;
        space_bloc[8*i+4] =  auxi_var4;
        space_bloc[8*i+5] = auxi_var5;
        space_bloc[8*i+6] = auxi_var6;
        space_bloc[8*i+7] = auxi_var7;
        
        // Etape 3
        auxi_var0 = space_bloc[8*i+0] + space_bloc[8*i+1];
        auxi_var1 = space_bloc[8*i+0] - space_bloc[8*i+1];

        auxi_var2 = SQRT2*(space_bloc[8*i+2]*COS6 + space_bloc[8*i+3]*SIN6);
        auxi_var3 = SQRT2*(space_bloc[8*i+3]*COS6 - space_bloc[8*i+2]*SIN6);

        //auxi_var2 = (space_bloc[8*i+3]-space_bloc[8*i+2])*SIN6 + space_bloc[8*i+2]*ADD_CS6;
        //auxi_var3 = (-space_bloc[8*i+3]-space_bloc[8*i+2])*COS6 + space_bloc[8*i+2]*ADD_CS6;

        auxi_var4 = space_bloc[8*i+4] + space_bloc[8*i+6];
        auxi_var6 = space_bloc[8*i+4] - space_bloc[8*i+6];

        auxi_var7 = space_bloc[8*i+7] + space_bloc[8*i+5];
        auxi_var5 = space_bloc[8*i+7] - space_bloc[8*i+5];

        space_bloc[8*i+0] = auxi_var0;
        space_bloc[8*i+1] = auxi_var1;
        space_bloc[8*i+2] = auxi_var2;
        space_bloc[8*i+3] = auxi_var3;
        space_bloc[8*i+4] = auxi_var4;
        space_bloc[8*i+5] = auxi_var5;
        space_bloc[8*i+6] = auxi_var6;
        space_bloc[8*i+7] = auxi_var7;
        
        // Etape 4
        auxiliaire[8*i+0] = space_bloc[8*i+0]/SQRT8;
        auxiliaire[8*i+4] = space_bloc[8*i+1]/SQRT8;
        auxiliaire[8*i+2] = space_bloc[8*i+2]/SQRT8;
        auxiliaire[8*i+6] = space_bloc[8*i+3]/SQRT8;

        auxiliaire[8*i+7] = (space_bloc[8*i+7] - space_bloc[8*i+4])/SQRT8;
        auxiliaire[8*i+3] = SQRT2*space_bloc[8*i+5]/SQRT8;
        auxiliaire[8*i+5] = SQRT2*space_bloc[8*i+6]/SQRT8;
        auxiliaire[8*i+1] = (space_bloc[8*i+7] + space_bloc[8*i+4])/SQRT8;
    }
    //DCT sur chaque colonne de la matrice intermédiaire.
    for(uint8_t i = 0; i < 8; i++){
    
        // Etape 1
        auxi_var0 = auxiliaire[8*0+i] + auxiliaire[8*7+i];
        auxi_var7 = auxiliaire[8*0+i] - auxiliaire[8*7+i];

        auxi_var1 = auxiliaire[8*1+i] + auxiliaire[8*6+i];
        auxi_var6 = auxiliaire[8*1+i] - auxiliaire[8*6+i];

        auxi_var2 = auxiliaire[8*2+i] + auxiliaire[8*5+i];
        auxi_var5 = auxiliaire[8*2+i] - auxiliaire[8*5+i];

        auxi_var3 = auxiliaire[8*3+i] + auxiliaire[8*4+i];
        auxi_var4 = auxiliaire[8*3+i] - auxiliaire[8*4+i];

        auxiliaire[8*0+i] = auxi_var0;
        auxiliaire[8*1+i] = auxi_var1;
        auxiliaire[8*2+i] = auxi_var2;
        auxiliaire[8*3+i] = auxi_var3;
        auxiliaire[8*4+i] = auxi_var4;
        auxiliaire[8*5+i] = auxi_var5;
        auxiliaire[8*6+i] = auxi_var6;
        auxiliaire[8*7+i] = auxi_var7;
        
        // Etape 2
        auxi_var0 = auxiliaire[8*0+i] + auxiliaire[8*3+i];
        auxi_var3 = auxiliaire[8*0+i] - auxiliaire[8*3+i];

        auxi_var1 = auxiliaire[8*1+i] + auxiliaire[8*2+i];
        auxi_var2 = auxiliaire[8*1+i] - auxiliaire[8*2+i];

        auxi_var4 = auxiliaire[8*4+i]*COS3 + auxiliaire[8*7+i]*SIN3;
        auxi_var7 = auxiliaire[8*7+i]*COS3 - auxiliaire[8*4+i]*SIN3;
        //auxi_var4 = (space_bloc[8*7+i]-space_bloc[8*4+i])*SIN3 + space_bloc[8*4+i]*ADD_CS3;
        //auxi_var7 = (-space_bloc[8*7+i]-space_bloc[8*4+i])*COS3 + space_bloc[8*4+i]*ADD_CS3;

        auxi_var5 = auxiliaire[8*5+i]*COS1 + auxiliaire[8*6+i]*SIN1;
        auxi_var6 = auxiliaire[8*6+i]*COS1 - auxiliaire[8*5+i]*SIN1;
        //auxi_var5 = (space_bloc[8*6+i]-space_bloc[8*5+i])*SIN1 + space_bloc[8*5+i]*ADD_CS1;
        //auxi_var6 = (-space_bloc[8*6+i]-space_bloc[8*5+i])*COS1 + space_bloc[8*5+i]*ADD_CS1;

        auxiliaire[8*0+i] = auxi_var0;
        auxiliaire[8*1+i] = auxi_var1;
        auxiliaire[8*2+i] = auxi_var2;
        auxiliaire[8*3+i] = auxi_var3;
        auxiliaire[8*4+i] = auxi_var4;
        auxiliaire[8*5+i] = auxi_var5;
        auxiliaire[8*6+i] = auxi_var6;
        auxiliaire[8*7+i] = auxi_var7;
        
        // Etape 3
        auxi_var0 = auxiliaire[8*0+i] + auxiliaire[8*1+i];
        auxi_var1 = auxiliaire[8*0+i] - auxiliaire[8*1+i];

        auxi_var2 = SQRT2*(auxiliaire[8*2+i]*COS6 + auxiliaire[8*3+i]*SIN6);
        auxi_var3 = SQRT2*(auxiliaire[8*3+i]*COS6 - auxiliaire[8*2+i]*SIN6);
        //auxi_var2 = (space_bloc[8*3+i]-space_bloc[8*2+i])*SIN6 + space_bloc[8*2+i]*ADD_CS6;
        //auxi_var3 = (-space_bloc[8*3+i]-space_bloc[8*2+i])*COS6 + space_bloc[8*2+i]*ADD_CS6;

        auxi_var4 = auxiliaire[8*4+i] + auxiliaire[8*6+i];
        auxi_var6 = auxiliaire[8*4+i] - auxiliaire[8*6+i];

        auxi_var7 = auxiliaire[8*7+i] + auxiliaire[8*5+i];
        auxi_var5 = auxiliaire[8*7+i] - auxiliaire[8*5+i];

        auxiliaire[8*0+i] = auxi_var0;
        auxiliaire[8*1+i] = auxi_var1;
        auxiliaire[8*2+i] = auxi_var2;
        auxiliaire[8*3+i] = auxi_var3;
        auxiliaire[8*4+i] = auxi_var4;
        auxiliaire[8*5+i] = auxi_var5;
        auxiliaire[8*6+i] = auxi_var6;
        auxiliaire[8*7+i] = auxi_var7;

        // Etape 4
        freq_bloc[8*0+i] =  (auxiliaire[8*0+i]/SQRT8);
        freq_bloc[8*4+i] =(auxiliaire[8*1+i]/SQRT8);
        freq_bloc[8*2+i] = (auxiliaire[8*2+i]/SQRT8);
        freq_bloc[8*6+i] = (auxiliaire[8*3+i]/SQRT8);

        freq_bloc[8*7+i] =  ((auxiliaire[8*7+i] - auxiliaire[8*4+i])/SQRT8);
        freq_bloc[8*3+i] =  (SQRT2*auxiliaire[8*5+i]/SQRT8);
        freq_bloc[8*5+i] =  (SQRT2*auxiliaire[8*6+i]/SQRT8);
        freq_bloc[8*1+i] = ((auxiliaire[8*7+i] + auxiliaire[8*4+i])/SQRT8);
    }
}
