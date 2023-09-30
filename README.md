Ensimag 1A Projet C - TP 2021/22
============================

Projet académique


Objective : Le but était de réaliser un encodeur d'image en .ppm ou .pgm vers .jpg


Team : Equipe de 3  
Time Spent : 3 semaines entière    
Technologies used:
- C



# Notre encodeur JPEG

Le projet arrive à encoder parfaitement les images ppm et pgm quelque soit le downsampling, une partie de l'encodage progressive à aussi été effectué


# Fonctionnement des modules et répartition du travail

## Les modules
    On a rajouté les modules dct.h, downsampling.h, mcu.h, ppm_reader.h, rle.h.

    dct.h s'occupe de faire la décomposition de Fourier discrète. On lui donne notre bloc, ainsi q'un bloc vide et la longueur du bloc (8). Il remplie le bloc vide avec les coefficients de la décomposition.

    downsampling.h s'occupe de l'échantillonnage.

    mcu.h contient le nécessaire afin de réaliser la quantification et le parcours zigzag.

    ppm_reader.h va lire le fichier ppm, couper en bloc et renvoyer les mcu.

    rle.h va s'occuper de la compression sans perte, elle prend un mcu et écrit dans le flux le codage (pour l'instant elle ne prend qu'un bloc mais ça va venir ...).
## Structures de données

### Stucture image

<img src="/readme/structure_image.png" width="800">

### Structure jpeg

<img src="/readme/structure_jpeg.png" width="800">

### Implémentation de la matrice.

    Nos matrices 8*8 cases sont implémentées par des vecteurs 64 cases, pour être plus rapide dans les parcours séquentielles.
## Diagramme de Gantt : Semaine I

<img src="/readme/Grantt.png" width="800">

## Diagramme de Gantt : Semaine II

<img src="/readme/GrantII.png" width="800">
