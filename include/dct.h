#ifndef _DCT_H_
#define _DCT_H_
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

extern float coefficient(uint8_t xi);

/*
Retourne un tableau les valeurs de cos. Tableau alloué dynamiquement
l'utilisateur doit liberer la mémoire lui même.
*/
extern float* set_cos_table();

/*
Prend une matrice freq_bloc puis la remplie avec les coefficents de la dct de la matrice space_bloc.
Version de base qui sert de référence.
*/
extern void compute_dct_naive(int32_t *space_bloc, int32_t* freq_bloc);

/*
Prend une matrice freq_bloc puis la remplie avec les coefficents de la dct de la matrice space_bloc.
On pré-calcul les valeurs du cosinus à l'exterieur et on passe le tableau de valeurs à la fonction.
*/
extern void compute_dct(int32_t *space_bloc, int32_t* freq_bloc, float *cos_table);

/*
Prend une matrice freq_bloc puis la remplie avec les coefficents de la dct de la matrice space_bloc.
On pré-calcul les valeurs du cosinus à l'exterieur et on passe le tableau de valeurs à la fonction.
On fait moins de multiplication en factorisant un des deux cosinus.
*/
extern void compute_dct_less_op(int32_t *space_bloc, int32_t* freq_bloc, float *cos_table);

/*
Renvoie un tableau des coefficients nécessaire à la méthode de calcul dct-II.
C_i = cos_table[i]
*/
extern float* set_coeff_dct2();

/*
Prend une matrice freq_bloc puis la remplie avec les coefficents de la dct de la matrice space_bloc.
On pré-calcul les valeurs du cosinus à l'exterieur et on passe le tableau de valeurs à la fonction.
On fait moins d'opération avec la méthode de calcul dct-II.
Formule et matrice trouvées sur la page wiki de la dct.
*/
extern void dct_2(int32_t *space_bloc, int32_t* freq_bloc);

/*
Prend une matrice freq_bloc puis la remplie avec les coefficents de la dct de la matrice bloc.
Des macro sont définies au début de fichier (COS1, SIN1, COS2, SIN2, COS6, SIN6, SQRT2, SQRT8).
On fait moins d'opération avec la méthode de calcul loeffler.
Description de l'algo sur : https://www.ijaiem.org/volume3issue5/IJAIEM-2014-05-31-117.pdf (avec une erreur sur le schéma, oubli du sqrt2 pour le bloc C6).
*/
extern void loeffler(int32_t *bloc, int32_t* freq_bloc);
#endif /* DCT_H */
