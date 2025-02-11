#include "model/Model.hpp"
#include "model/Piece.hpp"
#include <climits>
#include <iostream>

// Initialize static member
vector<bool> Model::completedLevels(3, false);  // 3 levels, all initially not completed

Model::Model(int w, int h) :
    grid(w, h),
    pieces(),
    positions(),
    piecesPosees(),
    pieceActive(0),
    pieceADeplacer(0),
    width(w),
    height(h) {
    grid.addObserver(this);
}

Model::~Model() {
    // Se retirer comme observateur de la grille
    grid.removeObserver(this);

    // Nettoyer les ressources
    pieces.clear();
    positions.clear();
    initialPositions.clear();
    initialCoordinates.clear();
    piecesPosees.clear();
    zonesArrivee.clear();
    zonesAutorisees.clear();

    // Supprimer tous les observateurs
    removeAllObservers();
}

// Gestion des pièces
void Model::ajouterPiece(const vector<pair<int, int>>& coords) {
    pieces.push_back(PieceConcrete(coords));
    positions.push_back(make_pair(-1, -1));
    piecesPosees.push_back(false);
    notifyObservers();
}

void Model::setPieceActive(size_t index) {
    if (index < pieces.size()) {
        pieceActive = index;
        // Pas besoin de notifier car pas de changement visuel
    }
}

const PieceConcrete& Model::getPieceActive() const {
    if (pieces.empty()) {
        throw runtime_error("Aucune pièce disponible");
    }
    return pieces[pieceActive];
}

size_t Model::getPieceActiveIndex() const {
    return pieceActive;
}

size_t Model::getNbPieces() const {
    return pieces.size();
}

const vector<pair<int, int>>& Model::getPieceCoords(size_t index) const {
    if (index >= pieces.size()) {
        throw runtime_error("Index de pièce invalide");
    }
    return pieces[index].getCoordinates();
}

bool Model::isPiecePosee(size_t index) const {
    if (index >= piecesPosees.size()) {
        return false;
    }
    return piecesPosees[index];
}

pair<int, int> Model::getPosition(size_t index) const {
    if (index >= positions.size()) {
        return make_pair(-1, -1);
    }
    return positions[index];
}

// Opérations sur la pièce active
bool Model::verifierCollision(int x, int y, size_t pieceIndex) const {
    for (size_t i = 0; i < pieces.size(); ++i) {
        if (i != pieceIndex && piecesPosees[i]) {
            const vector<pair<int, int>>& coords = pieces[i].getCoordinates();
            for (const pair<int, int>& coord : coords) {
                int px = positions[i].first + coord.first;
                int py = positions[i].second + coord.second;
                for (const pair<int, int>& activeCoord : pieces[pieceIndex].getCoordinates()) {
                    int ax = x + activeCoord.first;
                    int ay = y + activeCoord.second;
                    if (px == ax && py == ay) {
                        return true;  // Collision détectée
                    }
                }
            }
        }
    }
    return false;  // Pas de collision
}

bool Model::placerPiece(int x, int y) {
    if (pieces.empty() || pieceActive >= pieces.size()) {
        return false;
    }

    // Vérifier si une autre pièce occupe déjà cette position
    if (verifierCollision(x, y, pieceActive)) {
        return false;
    }

    // Sauvegarder la nouvelle position
    positions[pieceActive] = make_pair(x, y);

    // Essayer de placer la pièce
    grid.clear();  // Nettoyer la grille
    for (size_t i = 0; i < pieces.size(); ++i) {
        if (piecesPosees[i] && i != pieceActive) {
            grid.placePiece(pieces[i], positions[i].first, positions[i].second);
        }
    }

    if (grid.placePiece(pieces[pieceActive], x, y)) {
        piecesPosees[pieceActive] = true;

        // Vérifier la victoire après chaque placement
        if (pieceActive == pieceADeplacer && verifierVictoire()) {
            cout << "🎉 Félicitations ! Vous avez gagné !" << endl;
        }

        return true;
    }

    // Si échec, réinitialiser la position
    positions[pieceActive] = make_pair(-1, -1);
    piecesPosees[pieceActive] = false;
    redessinerGrille(true);
    return false;
}

// Nouvelle méthode utilitaire pour appliquer une transformation
bool Model::appliquerTransformation(PieceOperateur& operateur) {
    // Vérifications préliminaires
    if (pieces.empty()) {
        return false;
    }
    if (pieceActive >= pieces.size()) {
        return false;
    }
    if (!piecesPosees[pieceActive]) {
        return false;
    }

    // Sauvegarder l'état actuel
    PieceConcrete pieceOriginale = pieces[pieceActive];
    pair<int, int> positionOriginale = positions[pieceActive];

    try {
        // Appliquer la transformation en utilisant le Visitor
        pieces[pieceActive].accept(operateur);

        // Vérifier si la transformation est valide
        if (!verifierTransformation()) {
            pieces[pieceActive] = pieceOriginale;
            positions[pieceActive] = positionOriginale;
            redessinerGrille(true);
            return false;
        }

        // Vérifier si la nouvelle position est dans une zone autorisée
        bool positionValide = true;
        for (const pair<int, int>& coord : pieces[pieceActive].getCoordinates()) {
            int newX = positions[pieceActive].first + coord.first;
            int newY = positions[pieceActive].second + coord.second;
            if (!estZoneAutorisee(newX, newY)) {
                positionValide = false;
                break;
            }
        }

        if (!positionValide) {
            pieces[pieceActive] = pieceOriginale;
            positions[pieceActive] = positionOriginale;
            redessinerGrille(true);
            return false;
        }

        // Vérifier la victoire après la transformation
        if (pieceActive == pieceADeplacer && verifierVictoire()) {
            cout << "🎉 Félicitations ! Vous avez gagné !" << endl;
            setLevelCompleted(1);  // Marquer le niveau comme complété
        }

        notifyObservers();
        return true;
    }
    catch (const exception& e) {
        // En cas d'erreur, restaurer l'état original
        pieces[pieceActive] = pieceOriginale;
        positions[pieceActive] = positionOriginale;
        redessinerGrille(true);
        return false;
    }
}

// Simplifier les méthodes de transformation
bool Model::deplacerPiece(OrientationDeplacement direction) {
    if (pieces.empty() || pieceActive >= pieces.size() || !piecesPosees[pieceActive]) {
        return false;
    }

    // Créer l'opérateur de déplacement
    OperateurDeplacement deplacement(pieces[pieceActive], positions[pieceActive], direction);
    
    // Utiliser la méthode appliquerTransformation
    return appliquerTransformation(deplacement);
}

bool Model::symetriePiece(OrientationSymetrie sens) {
    if (pieces.empty() || pieceActive >= pieces.size() || !piecesPosees[pieceActive]) {
        return false;
    }

    // Créer l'opérateur de symétrie
    OperateurSymetrie symetrie(pieces[pieceActive], positions[pieceActive], sens);
    
    // Utiliser la méthode appliquerTransformation
    return appliquerTransformation(symetrie);
}

bool Model::rotationPiece(OrientationRotation sens) {
    if (pieces.empty() || pieceActive >= pieces.size() || !piecesPosees[pieceActive]) {
        return false;
    }

    // Créer l'opérateur de rotation
    OperateurRotation rotation(pieces[pieceActive], positions[pieceActive], sens);
    
    // Utiliser la méthode appliquerTransformation
    return appliquerTransformation(rotation);
}

// Nouvelle méthode utilitaire pour vérifier la validité d'une transformation
bool Model::verifierTransformation() {
    grid.clear();
    
    // Replacer toutes les pièces sauf la pièce active
    for (size_t i = 0; i < pieces.size(); ++i) {
        if (piecesPosees[i] && i != pieceActive) {
            if (!grid.placePiece(pieces[i], positions[i].first, positions[i].second)) {
                return false;
            }
        }
    }

    // Vérifier si la pièce active peut être placée dans sa nouvelle configuration
    return grid.placePiece(pieces[pieceActive], positions[pieceActive].first, positions[pieceActive].second);
}

// Gestion de la grille
void Model::effacerGrille() {
    grid.clear();
    for (size_t i = 0; i < pieces.size(); ++i) {
        positions[i] = make_pair(-1, -1);
        piecesPosees[i] = false;
    }
    notifyObservers();
}

const Grid& Model::getGrille() const {
    return grid;
}

// Méthodes de l'Observer pattern
void Model::update() {
    // Ne rien faire car la grille n'a pas besoin de nous notifier
}

// Méthodes utilitaires
void Model::redessinerGrille(bool notify) {
    grid.clear();
    for (size_t i = 0; i < pieces.size(); ++i) {
        if (piecesPosees[i]) {
            grid.placePiece(pieces[i], positions[i].first, positions[i].second);
        }
    }
    if (notify) {
        notifyObservers();
    }
}

void Model::setZoneArrivee(int x, int y) {
    viderZonesArrivee();
    ajouterZoneArrivee(x, y);
}

void Model::setPieceADeplacer(size_t index) {
    if (index < pieces.size()) {
        pieceADeplacer = index;
        notifyObservers();
    }
}

pair<int, int> Model::getZoneArrivee() const {
    if (zonesArrivee.empty()) {
        return make_pair(-1, -1);
    }
    return *zonesArrivee.begin();  // Retourne la première zone pour compatibilité
}

size_t Model::getPieceADeplacer() const {
    return pieceADeplacer;
}

bool Model::isPartieGagnee() const {
    return verifierVictoire();
}

bool Model::verifierVictoire() const {
    // Si la pièce n'est pas posée, pas de victoire possible
    if (!piecesPosees[pieceADeplacer]) {
        return false;
    }

    // Vérifier que toutes les parties de la pièce sont sur des zones d'arrivée
    const vector<pair<int, int>>& coords = pieces[pieceADeplacer].getCoordinates();
    const pair<int, int>& pos = positions[pieceADeplacer];

    // Pour le niveau 2 (pièce en forme de P avec 4 cases), toutes les cases doivent être sur des zones d'arrivée
    if (coords.size() == 4) {  // Si c'est la pièce en forme de P (niveau 2)
        // Vérifier que toutes les cases sont sur des zones d'arrivée
        for (const pair<int, int>& coord : coords) {
            int x = pos.first + coord.first;
            int y = pos.second + coord.second;

            if (!estZoneArrivee(x, y)) {
                return false;  // Si une seule case n'est pas sur une zone d'arrivée, ce n'est pas une victoire
            }
        }

        // Vérifier que la pièce a la bonne orientation (horizontale)
        int minX = INT_MAX, maxX = INT_MIN;
        int minY = INT_MAX, maxY = INT_MIN;

        // Trouver les dimensions de la pièce
        for (const pair<int, int>& coord : coords) {
            minX = std::min(minX, pos.first + coord.first);
            maxX = std::max(maxX, pos.first + coord.first);
            minY = std::min(minY, pos.second + coord.second);
            maxY = std::max(maxY, pos.second + coord.second);
        }

        int largeur = maxX - minX + 1;
        int hauteur = maxY - minY + 1;

        // La pièce doit être horizontale (largeur > hauteur)
        return largeur > hauteur;
    }

    // Pour les autres niveaux (1 et 3), une seule case sur une zone d'arrivée suffit
    for (const pair<int, int>& coord : coords) {
        int x = pos.first + coord.first;
        int y = pos.second + coord.second;

        if (estZoneArrivee(x, y)) {
            return true;  // Si une case est sur une zone d'arrivée, c'est une victoire
        }
    }

    return false;  // Aucune case sur une zone d'arrivée
}

void Model::ajouterZoneAutorisee(int x, int y) {
    zonesAutorisees.insert(make_pair(x, y));
    notifyObservers();
}

void Model::viderZonesAutorisees() {
    zonesAutorisees.clear();
    notifyObservers();
}

bool Model::estZoneAutorisee(int x, int y) const {
    return zonesAutorisees.find(make_pair(x, y)) != zonesAutorisees.end();
}

void Model::ajouterZoneArrivee(int x, int y) {
    zonesArrivee.insert(make_pair(x, y));
    notifyObservers();
}

void Model::viderZonesArrivee() {
    zonesArrivee.clear();
    notifyObservers();
}

bool Model::estZoneArrivee(int x, int y) const {
    return zonesArrivee.find(make_pair(x, y)) != zonesArrivee.end();
}
