#include "model/OperateurRotation.hpp"
#include "model/OperateurDeplacement.hpp"
#include "model/OperateurSymetrie.hpp"
#include "model/PieceConcrete.hpp"
#include <climits>
#include <cmath>
#include <algorithm>

OperateurRotation::OperateurRotation(Piece& source, const pair<int, int>& position, OrientationRotation s)
    : PieceOperateur(source, position), sens(s) {}

void OperateurRotation::accept(const PieceOperateur& v) const {
    v.visit(*const_cast<OperateurRotation*>(this));
}

void OperateurRotation::mapPosition(pair<int, int>& pos) const {
    // Trouver le centre de rotation (point de jonction)
    const vector<pair<int, int>>& coords = source.getCoordinates();
    pair<int, int> center = findRotationCenter(coords);

    // Coordonnées relatives au centre
    int relX = pos.first - center.first;
    int relY = pos.second - center.second;

    // Rotation
    int rotX, rotY;
    if (sens == OrientationRotation::HORAIRE) {
        rotX = -relY;
        rotY = relX;
    } else {
        rotX = relY;
        rotY = -relX;
    }

    // Retour aux coordonnées absolues
    pos.first = rotX + center.first;
    pos.second = rotY + center.second;
}

void OperateurRotation::visit(PieceConcrete& p) const {
    try {
        // Trouver le centre de rotation
        const vector<pair<int, int>>& coords = p.getCoordinates();
        pair<int, int> center = findRotationCenter(coords);

        // Appliquer la rotation à chaque coordonnée
        vector<pair<int, int>> newCoords;
        for (const pair<int, int>& coord : coords) {
            pair<int, int> rotatedCoord = coord;
            
            // Coordonnées relatives au centre
            int relX = rotatedCoord.first - center.first;
            int relY = rotatedCoord.second - center.second;

            // Rotation
            if (sens == OrientationRotation::HORAIRE) {
                rotatedCoord.first = center.first - relY;
                rotatedCoord.second = center.second + relX;
            } else {
                rotatedCoord.first = center.first + relY;
                rotatedCoord.second = center.second - relX;
            }

            newCoords.push_back(rotatedCoord);
        }

        // Mettre à jour les coordonnées de la pièce
        p.coordinates = newCoords;
    }
    catch (const exception& e) {
        throw runtime_error("Erreur lors de la visite de PieceConcrete: " + string(e.what()));
    }
}

void OperateurRotation::visit(OperateurDeplacement& op) const {
    // Appliquer la rotation à la position de l'opérateur
    pair<int, int> newPos = op.position;
    mapPosition(newPos);
    op.position = newPos;

    // Propager la transformation à la source
    op.source.accept(*this);
}

void OperateurRotation::visit(OperateurRotation& op) const {
    // Appliquer la rotation à la position de l'opérateur
    pair<int, int> newPos = op.position;
    mapPosition(newPos);
    op.position = newPos;

    // Propager la transformation à la source
    op.source.accept(*this);
}

void OperateurRotation::visit(OperateurSymetrie& op) const {
    // Appliquer la rotation à la position de l'opérateur
    pair<int, int> newPos = op.position;
    mapPosition(newPos);
    op.position = newPos;

    // Propager la transformation à la source
    op.source.accept(*this);
}

pair<int, int> OperateurRotation::findRotationCenter(const vector<pair<int, int>>& coords) const {
    if (coords.empty()) {
        throw invalid_argument("Les coordonnées ne peuvent pas être vides pour trouver le centre de rotation");
    }

    pair<int, int> center = coords[0];
    int maxNeighbors = 0;

    for (const pair<int, int>& point : coords) {
        int neighbors = 0;
        for (const pair<int, int>& other : coords) {
            if (point != other) {
                int dx = abs(point.first - other.first);
                int dy = abs(point.second - other.second);
                if (dx + dy == 1) {  // Points adjacents
                    neighbors++;
                }
            }
        }
        if (neighbors > maxNeighbors) {
            maxNeighbors = neighbors;
            center = point;
        }
    }

    if (maxNeighbors == 0) {
        throw invalid_argument("Impossible de trouver un point de jonction valide pour la rotation");
    }

    return center;
}

void OperateurRotation::afficher(ostream& os) const {
    os << "OperateurRotation at (" << position.first << ", " << position.second << ") ";
    os << "Sens: " << (sens == OrientationRotation::HORAIRE ? "Horaire" : "Anti-horaire");
    os << ". ";
    source.afficher(os);
}

ostream& operator<<(ostream& os, const OperateurRotation& op) {
    op.afficher(os);
    return os;
}
