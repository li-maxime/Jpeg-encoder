#ifndef _PPM_READER_H_
#define _PPM_READER_H_
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>






/*******************************************************/
/* Explication du principe et de l'optimisation mémoire*/
/*******************************************************/

/*
    Au lieu de stocker et lire entièrement l'image, on a décider de prendre des bandes de seulement v1*8 ligne de l'image à chaque fois.
    Cela nous permet d'optimiser la mémoire tout en évitant de devoir déplacer le curseur de lecture du fichier
    qui fait perdre du temps.
    Le fais de stocker 8*v1 ligne à chaque fois permet de réaliser fournir le bloc de matrice v1*h1 à sous échantillonner sans avoir besoin
    de relire le fichier .ppm, ou de stocker d'autre matrice intermédiaire.
*/

/********************/
/* Types de données */
/********************/

/*
    Structure de donnée contenant les information de l'image à convertir:
    son type, sa taille, ainsi que les structs des deux matrices sur laquelle on travaille.
*/
struct image{
    int col;
    int row;
    int type;
    int max_color;
    struct current_matrix *current_matrix;
    int h;
    int v;
    struct current_mcu *current_mcu;
    FILE *fichier;
};
/*
    Structure de donnée contenant les bandes de v1*8 lignes, la dimension de la bande, ainsi que le numéro de la bande actuelle
*/
struct current_matrix{
    int *matrix;
    int current_row;
    int size_x;
    int size_y;
    int total_size;
};
/*
    Structure de donnée contenant le bloc de matrice v1*h1 sur laquelle on va travailler, ses dimensions et son numéro
*/
struct current_mcu{
    int *mcu;
    int current_col;
    int size_x;
    int size_y;
    int total_size;
};

/****************************************/
/* Fonction de création et d'affichage */
/****************************************/

/*
    Ouvre le fichier de l'image.
    Initialise les informations des structures précédentes en fonction de l'image, h1 et v1.
    Alloue la mémoire aux différentes structures.
*/
extern struct image *convert_file(char *argv, int h, int v);

/*
    Permet de charger et stocker la prochaine matrice bande de 8*v1 ligne:
    Renvoie False si il ne peut plus charger de bande car la dernière a été déja charger.
    Sinon renvoie True et charge la prochaine bande
*/
extern bool next_matrix(struct image *image);

/*
    Charge le prochain bloc de v1*h1 mcu à partir des valeurs stocké dans current_matrix.
    Si la matrice chargé est la dernière de la bande, appelle next_matrix.
    Renvoie False si il n'y a plus de matrice à chargé et True sinon.
*/
extern bool new_mcu(struct image *image);


/*
    Affiche la bande actuelle présente dans le struct image
*/
extern void print_image(struct image *image);

/*
    Affiche le bloc MCU actuelle présente dans le struct image
*/
extern void print_mcu(struct image *image);

/*
    Détruit le struct image entièrement y compris les deux sous matrices
    et ferme le FILE.
*/
extern void free_image(struct image *image);

extern int img_get_col(struct image* img);

extern int img_get_row(struct image* img);

extern int * img_get_current_mcu(struct image* img);

#endif /* _PPM_READER_H_ */