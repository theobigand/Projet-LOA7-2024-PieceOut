#ifndef GRID_HPP
#define GRID_HPP

#include <vector>
#include <memory>
#include <iostream>
#include "Cell.hpp"
#include "Piece.hpp"
#include "Observer.hpp"

using namespace std;

/************************************************************
 * Pattern Composite pour la grille de jeu
 * - GridComponent : interface de base (Component)
 * - CellComponent : feuille (Leaf)
 * - GridRow : composite pour les lignes
 * - Grid : composite racine + Subject pour le pattern Observer
 ************************************************************/

 // Forward declaration pour le pattern Visitor
class GridVisitor;

// Interface Component du pattern Composite
// Définit le comportement commun pour tous les éléments de la grille
class GridComponent {
public:
    virtual ~GridComponent() = default;
    virtual void accept(GridVisitor& visitor) = 0;  // Pour le pattern Visitor
};

// Leaf (feuille) du pattern Composite
// Représente une cellule individuelle dans la grille
class CellComponent : public GridComponent {
private:
    unique_ptr<Cell> cell;  // Utilisation de unique_ptr au lieu d'un pointeur brut
public:
    CellComponent(int x, int y);
    ~CellComponent() override = default;  // Le unique_ptr gère la destruction automatiquement
    void accept(GridVisitor& visitor) override;
    Cell& getCell();
    const Cell& getCell() const;
};

// Composite pour une ligne de la grille
// Contient une collection de cellules (CellComponent)
class GridRow : public GridComponent {
private:
    vector<unique_ptr<GridComponent>> cells;  // Collection de cellules
public:
    GridRow(int width, int y);
    GridRow(const GridRow&) = delete;  // Supprime le constructeur de copie
    GridRow& operator=(const GridRow&) = delete;  // Supprime l'opérateur d'affectation
    void accept(GridVisitor& visitor) override;
    const vector<unique_ptr<GridComponent>>& getCells() const;
    CellComponent& getCellAt(int x);
    const CellComponent& getCellAt(int x) const;
};

/************************************************************
 * Pattern Visitor
 * Permet d'ajouter des opérations sur la structure composite
 * sans modifier les classes existantes
 ************************************************************/
class GridVisitor {
public:
    virtual ~GridVisitor() = default;
    virtual void visitGrid(class Grid& grid) = 0;
    virtual void visitRow(GridRow& row) = 0;
    virtual void visitCell(CellComponent& cell) = 0;
};

/************************************************************
 * Pattern Observer + Composite Root
 * La grille est à la fois :
 * - La racine du pattern Composite (contient les lignes)
 * - Un Subject du pattern Observer (notifie des changements)
 ************************************************************/
class Grid : public GridComponent, public Subject {
private:
    int width;   // Largeur de la grille
    int height;  // Hauteur de la grille
    vector<unique_ptr<GridComponent>> rows;  // Collection de lignes

public:
    Grid(int width, int height);
    ~Grid() override;  // Destructeur virtuel explicite
    Grid(const Grid&) = delete;  // Supprime le constructeur de copie
    Grid& operator=(const Grid&) = delete;  // Supprime l'opérateur d'affectation
    void accept(GridVisitor& visitor) override;

    // Méthodes de manipulation des pièces
    bool placePiece(const Piece& piece, int x, int y);
    bool canPlacePiece(const Piece& piece, int x, int y) const;
    void clear();
    void afficher(ostream& os) const;
};

#endif