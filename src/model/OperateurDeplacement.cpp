#include "model/OperateurDeplacement.hpp"
#include "model/OperateurRotation.hpp"
#include "model/OperateurSymetrie.hpp"
#include "model/PieceConcrete.hpp"

OperateurDeplacement::OperateurDeplacement(Piece& source, const pair<int, int>& position, OrientationDeplacement d)
    : PieceOperateur(source, position), direction(d) {}

void OperateurDeplacement::accept(const PieceOperateur& v) const {
    // Le const_cast est nécessaire ici car le pattern Visitor modifie l'objet visité
    // mais la méthode accept doit rester const pour respecter le contrat de Piece
    OperateurDeplacement* nonConstThis = const_cast<OperateurDeplacement*>(this);
    v.visit(*nonConstThis);
}

void OperateurDeplacement::mapPosition(pair<int, int>& pos) const {
    // Applique le déplacement selon la direction
    switch (direction) {
    case OrientationDeplacement::NORD:
        pos.second--;  // Déplacement vers le haut (y diminue)
        break;
    case OrientationDeplacement::SUD:
        pos.second++;  // Déplacement vers le bas (y augmente)
        break;
    case OrientationDeplacement::OUEST:
        pos.first--;   // Déplacement vers la gauche (x diminue)
        break;
    case OrientationDeplacement::EST:
        pos.first++;   // Déplacement vers la droite (x augmente)
        break;
    }
}

void OperateurDeplacement::visit(PieceConcrete& p) const {
    // Optimisation : réserve la mémoire pour éviter les réallocations
    const vector<pair<int, int>>& coords = p.getCoordinates();
    vector<pair<int, int>> newCoords;
    newCoords.reserve(coords.size());

    // Applique le déplacement à chaque coordonnée
    for (const pair<int, int>& coord : coords) {
        pair<int, int> newCoord = coord;
        mapPosition(newCoord);
        newCoords.push_back(newCoord);
    }
    // Utilise move pour éviter la copie lors de l'affectation
    p.coordinates = std::move(newCoords);
}

void OperateurDeplacement::visit(OperateurDeplacement& op) const {
    // Déplace d'abord l'opérateur lui-même
    pair<int, int> newPos = op.getPosition();
    mapPosition(newPos);
    op.getPositionMutable() = newPos;
    // Puis propage la transformation à la source
    op.getSourceMutable().accept(*this);
}

void OperateurDeplacement::visit(OperateurRotation& op) const {
    // Même logique que pour OperateurDeplacement
    pair<int, int> newPos = op.getPosition();
    mapPosition(newPos);
    op.getPositionMutable() = newPos;
    op.getSourceMutable().accept(*this);
}

void OperateurDeplacement::visit(OperateurSymetrie& op) const {
    // Même logique que pour OperateurDeplacement
    pair<int, int> newPos = op.getPosition();
    mapPosition(newPos);
    op.getPositionMutable() = newPos;
    op.getSourceMutable().accept(*this);
}

void OperateurDeplacement::afficher(ostream& os) const {
    os << "Deplacement(" << position.first << "," << position.second << ") ";
    source.afficher(os);
}
