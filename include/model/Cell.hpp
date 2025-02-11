#ifndef CELL_HPP
#define CELL_HPP

#include "Piece.hpp"
using namespace std;

/************************************************************
 * Classe Cell
 * Représente une cellule individuelle dans la grille
 *
 * Responsabilités :
 * - Maintenir l'état d'occupation de la cellule
 * - Gérer la pièce qui occupe la cellule
 * - Connaître sa position dans la grille
 ************************************************************/
class Cell {
private:
    int x;          // Position X dans la grille
    int y;          // Position Y dans la grille
    Piece* occupant;  // Pointeur vers la pièce qui occupe la cellule (nullptr si vide)

public:
    // Constructeur : initialise une cellule à une position donnée
    Cell(int x, int y);

    // Méthodes d'accès
    bool isOccupied() const;              // Vérifie si la cellule est occupée
    void setOccupant(Piece* piece);       // Place une pièce dans la cellule
    void removeOccupant();                // Retire la pièce de la cellule
    Piece* getOccupant() const;           // Récupère la pièce qui occupe la cellule

    // Accesseurs pour la position
    int getX() const;                     // Récupère la coordonnée X
    int getY() const;                     // Récupère la coordonnée Y
};

#endif