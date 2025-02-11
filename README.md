# Projet PieceOut

Un jeu de puzzle où il faut déplacer des pièces pour atteindre un objectif spécifique à chaque niveau.

## Compilation et Exécution

Dans une console Unix, après avoir décompressé l'archive :

```bash
make gui && ./gui
```

## Contrôles

- Clic gauche : Sélectionner/Déplacer une pièce
- Flèches : Déplacer la pièce active
- R : Rotation horaire
- E : Rotation anti-horaire
- S : Symétrie verticale
- H : Symétrie horizontale
- Tab : Changer de pièce active
- M : Menu
- Esc : Quitter

## Dépendances requises

- g++ avec support C++11
- SFML 2.x (ne pas avoir SFML 3 !)
- make

Sur Ubuntu/Debian, installez les dépendances avec :
```bash
sudo apt-get install g++ libsfml-dev make
```
