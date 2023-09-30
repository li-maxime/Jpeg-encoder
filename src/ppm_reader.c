#include <stdlib.h>
#include <ppm_reader.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>


/*
    Ouvre le fichier de l'image.
    Initialise les informations des structures précédentes en fonction de l'image, h1 et v1.
    Alloue la mémoire aux différentes structures.
*/
struct image *convert_file(char *argv, int h, int v){
    // Allocation mémoires des trois structures
    uint8_t received;
    char *got;
    struct image *image = malloc(sizeof(struct image));
    image->current_matrix = malloc(sizeof(struct current_matrix));
    struct current_matrix *matrix = image->current_matrix;
    image->current_mcu = malloc(sizeof(struct current_mcu));
    struct current_mcu *mcu = image->current_mcu;
    char buffer[20];
    // Initialisation valeur stuct image, current_mcu et current_matrix
    image->fichier = fopen(argv, "r");
    got = fgets(buffer, 20, image->fichier);
    received = sscanf(buffer, "P%i", &image->type);
    if (received != 1){
        printf("Error : cannot read this image, first line is not P5 or P6\n");
        return NULL;
    }
    matrix->current_row = 0;
    
    if(image->type == 6)
    {
        image->v = v;
        image->h = h;
    }
    else
    {
        image->v = 1;
        image->h = 1;
    }

    mcu->size_x = 8*image->h;
    mcu->size_y = 8*image->v;
    mcu->total_size = mcu->size_x * mcu->size_y;
    matrix->size_y = 8*image->v;
    //Lecture de l'en-tête du fichier ppm 

    got = fgets(buffer, 20, image->fichier);
    received = sscanf(buffer, "%d %d", &image->col, &image->row);
    if (received != 2){
        printf("Error : cannot read this image, second line  does not contain height and lenght\n");
        return NULL;
    }
    got = fgets(buffer, 20, image->fichier);
    (void)got;
    received = sscanf(buffer, "%d", &image->max_color);
    if (received != 1){
        printf("Error : cannot read this image, third line does not contain max color\n");
        return NULL;
    }
    matrix->size_x = (image->col%(image->h*8) == 0) ? image->col/(image->h*8) : (image->col/(image->h*8)+1);
    matrix->size_x = matrix->size_x * 8 * image->h;
    matrix->total_size = matrix->size_x * matrix->size_y;
    //Allocation mémoires des 
    matrix->matrix = malloc(matrix->total_size * sizeof(int));
    mcu->mcu = malloc(mcu->total_size * sizeof(int));
    if (image->fichier == NULL){
        //printf("Le fichier de sortie n'existe pas");
    }
    return image;
}


/*
    Permet de charger et stocker la prochaine matrice bande de 8*v1 ligne:
    Renvoie False si il ne peut plus charger de bande car la dernière a été déja charger.
    Sinon renvoie True et charge la prochaine bande
*/
bool next_matrix(struct image *image){
    struct current_matrix *current_matrix = image->current_matrix;
    if (current_matrix->current_row >= image->row){                 //REtourne False si la dernière bande à déja été chargé
        return false;
    }
    int *matrix = current_matrix->matrix;
    int valeur = 0;
    int type = (image->type == 5) ? 1 : 3;
    int i = 0;
    image->current_mcu->current_col = 0;
    while(i < current_matrix->total_size){
        if ((fread(&valeur, type, 1, image->fichier)) == 1){        //Concatène les valeur RGB dans l'ordre inverse (BGR) 
            matrix[i] = valeur;                                     //dans une seul case dans le cas d'un fichier P6
            i++;
            while((i % (current_matrix->size_x)) >= image->col ){   //Ajoutes les colonnes manquantes pour avoir des bloc MCU (v1*8)*(h1*8)
                matrix[i] = valeur;
                i++;
            }
        }
        else{                                                       //Remplis les lignes manqauntes à la fin en copiant les valeur de la ligne précédente
            matrix[i] = matrix[i-current_matrix->size_x];
            i++;
        }    
    }
    current_matrix->current_row += current_matrix->size_y;
    return true;
}


/*
    Charge le prochain bloc de v1*h1 mcu à partir des valeurs stocké dans current_matrix.
    Si la matrice chargé est la dernière de la bande, appelle next_matrix.
    Renvoie False si il n'y a plus de matrice à chargé et True sinon.
*/
bool new_mcu(struct image *image){
    struct current_mcu *current_mcu = image->current_mcu;
    int size = image->current_matrix->size_x;
    int *matrix = image->current_matrix->matrix;
    int *mcu = current_mcu->mcu;
    int start = current_mcu->current_col;
    for (int i = 0; i<current_mcu->total_size; i++){
        mcu[i] = matrix[start + i%current_mcu->size_x + i/(current_mcu->size_x)*size];
    }
    current_mcu->current_col += current_mcu->size_x;
    if (current_mcu->current_col>=image->current_matrix->size_x){       //Si la MCU est la dèrnière de la bande alors appelle next_matrix
        return next_matrix(image);
    }
    else{
        return true;
    }
}


/*
    Affiche la bande actuelle présente dans le struct image
*/
void print_image(struct image *image){
    int *ptr = image-> current_matrix->matrix;
    for (int i = 0; i<image-> current_matrix->total_size; i++){
        if (i%(image-> current_matrix->size_x) == 0){
            printf("\n \n");
        }
        printf("%02x,", ptr[i]);
    }
    printf("\n");
}


/*
    Affiche le bloc MCU actuelle présente dans le struct image
*/
void print_mcu(struct image *image){
    int *ptr = image-> current_mcu->mcu;
    for (int i = 0; i<image-> current_mcu->total_size; i++){
        if (i%(image-> current_mcu->size_x) == 0){
            printf("\n \n");
        }
        printf("%02x,", ptr[i]);
    }
    printf("\n");
}


/*
    Détruit le struct image entièrement y compris les deux sous matrices
    et ferme le FILE.
*/
void free_image(struct image *image){
    free(image->current_matrix->matrix);
    free(image->current_matrix);
    free(image->current_mcu->mcu);
    free(image->current_mcu);
    fclose(image->fichier);
    free(image);
}

int img_get_col(struct image* img){
    return img->col;
}

int img_get_row(struct image* img){
    return img->row;
}

int * img_get_current_mcu(struct image* img){
    return img->current_mcu->mcu;
}

/*
int main(int argc, char **argv){
    double time_spent =0.0;
    clock_t begin = clock();
    struct image *image = convert_file(argv[1], 1, 1);
    int i = 1;
    int j = 0;
    next_matrix(image);
    while (i == 1){
        i = new_mcu(image);
        j++;
        ////printf("%i\n", j);
    }
    free_image(image);
    clock_t end = clock();
    time_spent = (double)(end - begin)/ CLOCKS_PER_SEC; 
    //printf("used time = %f seconds", time_spent);
    return EXIT_SUCCESS;
}
*/

