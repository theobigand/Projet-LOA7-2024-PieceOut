#include "model/PieceConcrete.hpp"
#include "model/PieceOperateur.hpp"

PieceConcrete::PieceConcrete(const vector<pair<int, int>>& coords)
    : coordinates(coords) {}

const vector<pair<int, int>>& PieceConcrete::getCoordinates() const {
    return coordinates;
}

void PieceConcrete::trigger(const pair<int, int>& coord, const Piece& origin) {
    if (const PieceOperateur* op = dynamic_cast<const PieceOperateur*>(&origin)) {
        accept(*op);
    }
}

void PieceConcrete::accept(const PieceOperateur& v) const {
    v.visit(*const_cast<PieceConcrete*>(this));
}

void PieceConcrete::afficher(ostream& os) const {
    os << "PieceConcrete: ";
    for (const pair<int, int>& coord : coordinates) {
        os << "(" << coord.first << ", " << coord.second << ") ";
    }
}