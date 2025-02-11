#include "model/PieceOperateur.hpp"
#include "model/PieceConcrete.hpp"

PieceOperateur::PieceOperateur(Piece& s, const pair<int, int>& pos)
    : source(s), position(pos) {}

const vector<pair<int, int>>& PieceOperateur::getCoordinates() const {
    return source.getCoordinates();
}

void PieceOperateur::trigger(const pair<int, int>& relativePos, const Piece& origin) {
    if (relativePos == position) {
        origin.accept(*this);
    } else {
        source.trigger(relativePos, origin);
    }
}