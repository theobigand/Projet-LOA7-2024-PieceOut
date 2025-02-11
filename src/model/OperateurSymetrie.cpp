#include "model/OperateurSymetrie.hpp"
#include "model/OperateurDeplacement.hpp"
#include "model/OperateurRotation.hpp"
#include "model/PieceConcrete.hpp"
#include <stdexcept>

OperateurSymetrie::OperateurSymetrie(Piece& source, const pair<int, int>& position, OrientationSymetrie s)
    : PieceOperateur(source, position), sens(s) {}

void OperateurSymetrie::accept(const PieceOperateur& v) const {
    // Le const_cast est nécessaire ici car le pattern Visitor modifie l'objet visité
    // mais la méthode accept doit rester const pour respecter le contrat de Piece
    OperateurSymetrie* nonConstThis = const_cast<OperateurSymetrie*>(this);
    v.visit(*nonConstThis);
}

void OperateurSymetrie::mapPosition(pair<int, int>& pos) const {
    // Récupère les coordonnées pour trouver le centre de symétrie
    const vector<pair<int, int>>& coords = source.getCoordinates();
    const pair<int, int> center = findSymetryCenter(coords);

    // Applique la symétrie selon l'axe choisi
    if (sens == OrientationSymetrie::VERTICALE) {
        // Symétrie par rapport à l'axe vertical : x devient (2*center.x - x)
        int dx = pos.first - center.first;
        pos.first = center.first - dx;
    } else {
        // Symétrie par rapport à l'axe horizontal : y devient (2*center.y - y)
        int dy = pos.second - center.second;
        pos.second = center.second - dy;
    }
}

void OperateurSymetrie::visit(PieceConcrete& p) const {
    // Optimisation : réserve la mémoire pour éviter les réallocations
    const vector<pair<int, int>>& coords = p.getCoordinates();
    const pair<int, int> center = findSymetryCenter(coords);

    vector<pair<int, int>> newCoords;
    newCoords.reserve(coords.size());

    // Applique la symétrie à chaque coordonnée
    for (const pair<int, int>& coord : coords) {
        pair<int, int> symetrieCoord = coord;
        
        // Calcule les coordonnées relatives au centre
        int relX = symetrieCoord.first - center.first;
        int relY = symetrieCoord.second - center.second;

        // Applique la symétrie selon l'axe choisi
        if (sens == OrientationSymetrie::VERTICALE) {
            symetrieCoord.first = center.first - relX;    // Inverse x
            symetrieCoord.second = center.second + relY;  // Garde y
        } else {
            symetrieCoord.first = center.first + relX;    // Garde x
            symetrieCoord.second = center.second - relY;  // Inverse y
        }

        newCoords.push_back(symetrieCoord);
    }

    // Utilise move pour éviter la copie lors de l'affectation
    p.coordinates = std::move(newCoords);
}

pair<int, int> OperateurSymetrie::findSymetryCenter(const vector<pair<int, int>>& coords) const {
    // Vérifie que les coordonnées ne sont pas vides
    if (coords.empty()) {
        throw invalid_argument("Les coordonnées ne peuvent pas être vides");
    }

    // Commence avec le premier point comme centre potentiel
    pair<int, int> center = coords[0];
    int maxNeighbors = 0;

    // Cherche le point avec le plus de voisins adjacents
    for (const pair<int, int>& point : coords) {
        int neighbors = 0;
        for (const pair<int, int>& other : coords) {
            if (point != other) {
                // Calcule la distance de Manhattan entre les points
                int dx = abs(point.first - other.first);
                int dy = abs(point.second - other.second);
                // Un point est adjacent si la distance est 1
                if (dx + dy == 1) {
                    neighbors++;
                }
            }
        }
        // Met à jour le centre si ce point a plus de voisins
        if (neighbors > maxNeighbors) {
            maxNeighbors = neighbors;
            center = point;
        }
    }

    // Vérifie qu'un point valide a été trouvé
    if (maxNeighbors == 0) {
        throw invalid_argument("Impossible de trouver un point de jonction valide");
    }

    return center;
}

void OperateurSymetrie::visit(OperateurDeplacement& op) const {
    // Applique la symétrie à la position de l'opérateur
    pair<int, int> newPos = op.getPosition();
    mapPosition(newPos);
    op.getPositionMutable() = newPos;
    // Propage la transformation à la source
    op.getSourceMutable().accept(*this);
}

void OperateurSymetrie::visit(OperateurRotation& op) const {
    // Même logique que pour OperateurDeplacement
    pair<int, int> newPos = op.getPosition();
    mapPosition(newPos);
    op.getPositionMutable() = newPos;
    op.getSourceMutable().accept(*this);
}

void OperateurSymetrie::visit(OperateurSymetrie& op) const {
    // Même logique que pour OperateurDeplacement
    pair<int, int> newPos = op.getPosition();
    mapPosition(newPos);
    op.getPositionMutable() = newPos;
    op.getSourceMutable().accept(*this);
}

void OperateurSymetrie::afficher(ostream& os) const {
    os << "Symetrie(" << position.first << "," << position.second << ") ";
    os << (sens == OrientationSymetrie::HORIZONTALE ? "H" : "V") << " ";
    source.afficher(os);
}
