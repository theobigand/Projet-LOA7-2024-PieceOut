#include "model/Cell.hpp"

Cell::Cell(int x, int y) : x(x), y(y), occupant(nullptr) {}

bool Cell::isOccupied() const {
    return occupant != nullptr;
}

void Cell::setOccupant(Piece* piece) {
    occupant = piece;
}

void Cell::removeOccupant() {
    occupant = nullptr;
}

Piece* Cell::getOccupant() const {
    return occupant;
}

int Cell::getX() const {
    return x;
}

int Cell::getY() const {
    return y;
} 