# Traitement d’Images BMP en C

## Description

Ce projet a été réalisé dans le cadre du module **TI202 - Algorithmique et Structures de Données 1** à l’Efrei Paris (année 2024-2025). Il s’agit d’un programme en langage C permettant de traiter des images numériques au format **BMP**, un format simple, non compressé, facilitant les manipulations de bas niveau.

Le projet prend en charge deux types d’images :
- Les **images en niveaux de gris (8 bits)**, permettant l’implémentation des traitements de base (négatif, seuil, luminosité) et des filtres convolutifs.
- Les **images en couleur (24 bits)**, pour lesquelles des traitements plus avancés sont réalisés (conversion YUV, égalisation d’histogramme couleur, convolution RGB).

Le tout est accessible via une **interface en ligne de commande**, simple, permettant à l'utilisateur de charger une image, appliquer des transformations et sauvegarder le résultat.

---

## Compilation

### Prérequis :
- Compilateur C (`gcc`)
- Optionnel : `cmake`

### Compilation manuelle :
```bash
gcc -std=c99 -Wall -Wextra \
    main.c \
    bmp8.c bmp24.c histogramme.c utils.c \
    -o bmp_tool -lm
```

---

## Exécution

```bash
./bmp_tool
```

Interface CLI :
```
1. Ouvrir une image
2. Sauvegarder une image
3. Appliquer un filtre
4. Afficher les informations de l'image
5. Quitter
```

---

## Fonctionnalités Implémentées

### Images 8 bits :
- Chargement et sauvegarde
- Affichage des métadonnées (taille, profondeur, etc.)
- Filtres :
  - Négatif
  - Luminosité (±)
  - Seuil/binarisation
  - Filtres convolutifs : box blur, flou gaussien, contours, emboss, netteté
- Égalisation d’histogramme (calcul de l’histogramme, CDF, remapping)

### Images 24 bits :
- Chargement et sauvegarde
- Conversion en niveaux de gris
- Filtres : négatif, luminosité, convolutions (idem 8 bits)
- Égalisation via conversion RGB ↔ YUV

---

## Bugs Connus

-  Aucun support des fichiers BMP compressés
-  L’égalisation en 24 bits via YUV peut modifier légèrement les couleurs (approximations)
-  Absence de vérification de la profondeur (8 ou 24 bits) lors du chargement
-  Fichiers BMP malformés peuvent générer des erreurs fatales sans message explicite

---

##  Répartition des Tâches

- **Omar Snani** :
  - Partie 1 : Gestion des images 8 bits et filtres
  - Partie 3 : Égalisation d’histogramme
- **Teddy Rame** :
  - Partie 2 : Traitement des images couleur 24 bits

> Les deux membres ont collaboré activement à la résolution des problèmes, au débogage et à l’intégration des modules.

---

##  Conclusion

Ce projet nous a permis d’approfondir notre compréhension de la représentation mémoire des images, des opérations de manipulation de fichiers binaires en C et des algorithmes de traitement d’images (filtres, histogrammes, convolutions). Il constitue une base solide pour aller vers des applications plus complexes telles que la segmentation, la détection d’objets ou encore la compression.

