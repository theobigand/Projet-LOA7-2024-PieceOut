#ifndef MODEL_HPP
#define MODEL_HPP

#include "Grid.hpp"
#include "PieceConcrete.hpp"
#include "OperateurDeplacement.hpp"
#include "OperateurRotation.hpp"
#include "OperateurSymetrie.hpp"
#include "Observer.hpp"
#include <memory>
#include <vector>
#include <map>
#include <set>

using namespace std;

/************************************************************
 * Classe Model
 * Gère la logique du jeu et maintient l'état global
 *
 * Pattern Observer :
 * - Hérite de Subject pour notifier les vues des changements
 * - Hérite de Observer pour observer la grille
 *
 * Responsabilités :
 * - Gérer la grille de jeu
 * - Gérer les pièces et leurs transformations
 * - Maintenir l'état du jeu
 * - Notifier les observateurs des changements
 ************************************************************/
class Model : public Subject, public Observer {
private:
    Grid grid;                      // La grille de jeu
    vector<PieceConcrete> pieces;   // Collection de pièces disponibles
    vector<pair<int, int>> positions;  // Positions de chaque pièce
    vector<pair<int, int>> initialPositions;  // Store initial positions
    vector<vector<pair<int, int>>> initialCoordinates;  // Store initial coordinates/orientation of pieces
    vector<bool> piecesPosees;      // État de pose de chaque pièce
    size_t pieceActive;             // Index de la pièce active
    set<pair<int, int>> zonesArrivee;  // Positions des zones d'arrivée
    size_t pieceADeplacer;          // Index de la pièce qui doit atteindre la zone
    int width;                      // Largeur de la grille
    int height;                     // Hauteur de la grille
    set<pair<int, int>> zonesAutorisees;  // Zones où les déplacements sont autorisés
    pair<int, int> zoneArrivee;
    static vector<bool> completedLevels;  // Track which levels are completed

public:
    // Constructeur et destructeur
    Model(int width = 8, int height = 8);
    ~Model() override;

    // Gestion des zones de déplacement
    void ajouterZoneAutorisee(int x, int y);
    void viderZonesAutorisees();
    bool estZoneAutorisee(int x, int y) const;
    const set<pair<int, int>>& getZonesAutorisees() const { return zonesAutorisees; }

    // Gestion des zones d'arrivée
    void ajouterZoneArrivee(int x, int y);
    void viderZonesArrivee();
    bool estZoneArrivee(int x, int y) const;
    const set<pair<int, int>>& getZonesArrivee() const { return zonesArrivee; }

    // Gestion des dimensions
    int getNbColonnes() const { return width; }
    int getNbLignes() const { return height; }

    // Gestion des pièces
    void ajouterPiece(const vector<pair<int, int>>& coords);
    void setPieceActive(size_t index);
    const PieceConcrete& getPieceActive() const;
    size_t getPieceActiveIndex() const;
    size_t getNbPieces() const;
    const vector<pair<int, int>>& getPieceCoords(size_t index) const;
    bool isPiecePosee(size_t index) const;
    pair<int, int> getPosition(size_t index) const;

    // Gestion de la zone d'arrivée
    void setZoneArrivee(int x, int y);
    void setPieceADeplacer(size_t index);
    pair<int, int> getZoneArrivee() const;
    size_t getPieceADeplacer() const;
    bool isPartieGagnee() const;

    // Opérations sur la pièce active
    bool placerPiece(int x, int y);
    bool deplacerPiece(OrientationDeplacement direction);
    bool rotationPiece(OrientationRotation sens);
    bool symetriePiece(OrientationSymetrie sens);

    // Gestion de la grille
    void effacerGrille();
    const Grid& getGrille() const;

    // Méthode de l'Observer pattern
    void update() override;

    static void setLevelCompleted(int level) {
        if (completedLevels.size() < static_cast<size_t>(level)) {
            completedLevels.resize(level, false);
        }
        completedLevels[level - 1] = true;
    }

    static bool isLevelCompleted(int level) {
        return level > 0 && static_cast<size_t>(level) <= completedLevels.size() && completedLevels[level - 1];
    }

    // Store current positions as initial
    void storeInitialPositions() {
        initialPositions = positions;
        initialCoordinates.clear();
        for (const PieceConcrete& piece : pieces) {
            initialCoordinates.push_back(piece.getCoordinates());
        }
    }

    // Reset pieces to initial positions and orientations
    void resetToInitialPositions() {
        // Clear the grid first
        grid.clear();

        // Restore all pieces to their initial positions and orientations
        for (size_t i = 0; i < pieces.size(); ++i) {
            // Restore position
            positions[i] = initialPositions[i];
            // Restore orientation/coordinates
            pieces[i].coordinates = initialCoordinates[i];

            if (initialPositions[i].first != -1) {
                piecesPosees[i] = true;
                grid.placePiece(pieces[i], positions[i].first, positions[i].second);
            } else {
                piecesPosees[i] = false;
            }
        }

        // Reset active piece to the piece that needs to be moved
        pieceActive = pieceADeplacer;

        notifyObservers();
    }

private:
    // Méthodes utilitaires
    void redessinerGrille(bool notify = true);
    bool verifierVictoire() const;  // Vérifie si la pièce à déplacer a atteint la zone d'arrivée
    bool verifierCollision(int x, int y, size_t pieceIndex) const;  // Vérifie les collisions entre pièces
    bool verifierTransformation();  // Vérifie si une transformation est valide

    // Méthode utilitaire pour appliquer une transformation
    bool appliquerTransformation(PieceOperateur& operateur);
};

#endif
