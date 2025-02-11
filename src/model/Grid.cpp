#include "model/Grid.hpp"
#include <iostream>

// CellComponent implementation
CellComponent::CellComponent(int x, int y) : cell(unique_ptr<Cell>(new Cell(x, y))) {}

void CellComponent::accept(GridVisitor& visitor) {
    visitor.visitCell(*this);
}

Cell& CellComponent::getCell() {
    return *cell;
}

const Cell& CellComponent::getCell() const {
    return *cell;
}

// GridRow implementation
GridRow::GridRow(int width, int y) {
    for (int x = 0; x < width; ++x) {
        cells.push_back(unique_ptr<GridComponent>(new CellComponent(x, y)));
    }
}

void GridRow::accept(GridVisitor& visitor) {
    visitor.visitRow(*this);
    for (const unique_ptr<GridComponent>& cell : cells) {
        cell->accept(visitor);
    }
}

const vector<unique_ptr<GridComponent>>& GridRow::getCells() const {
    return cells;
}

CellComponent& GridRow::getCellAt(int x) {
    if (x >= 0 && x < static_cast<int>(cells.size())) {
        return static_cast<CellComponent&>(*cells[x]);
    }
    throw out_of_range("Index hors limites");
}

const CellComponent& GridRow::getCellAt(int x) const {
    if (x >= 0 && x < static_cast<int>(cells.size())) {
        return static_cast<const CellComponent&>(*cells[x]);
    }
    throw out_of_range("Index hors limites");
}

// Grid implementation
Grid::Grid(int width, int height) : width(width), height(height), rows(height) {
    for (int i = 0; i < height; ++i) {
        rows[i] = unique_ptr<GridComponent>(new GridRow(width, i));
    }
}

void Grid::accept(GridVisitor& visitor) {
    visitor.visitGrid(*this);
    for (unique_ptr<GridComponent>& row : rows) {
        row->accept(visitor);
    }
}

bool Grid::placePiece(const Piece& piece, int baseX, int baseY) {
    // Vérifier d'abord si la pièce peut être placée
    if (!canPlacePiece(piece, baseX, baseY)) {
        cout << "Impossible de placer la pièce en (" << baseX << "," << baseY << ") : ";

        // Vérifier pourquoi le placement est impossible
        for (const pair<int, int>& coord : piece.getCoordinates()) {
            int x = baseX + coord.first;
            int y = baseY + coord.second;

            if (x < 0 || x >= width || y < 0 || y >= height) {
                cout << "coordonnées (" << x << "," << y << ") hors limites" << endl;
                return false;
            }

            try {
                const GridRow& row = static_cast<const GridRow&>(*rows[y]);
                const CellComponent& cellComponent = row.getCellAt(x);
                if (cellComponent.getCell().isOccupied()) {
                    cout << "position (" << x << "," << y << ") déjà occupée" << endl;
                    return false;
                }
            }
            catch (const out_of_range&) {
                cout << "position (" << x << "," << y << ") invalide" << endl;
                return false;
            }
        }
        return false;
    }

    // Placer la pièce
    for (const pair<int, int>& coord : piece.getCoordinates()) {
        int x = baseX + coord.first;
        int y = baseY + coord.second;
        try {
            GridRow& row = static_cast<GridRow&>(*rows[y]);
            CellComponent& cellComponent = row.getCellAt(x);
            cellComponent.getCell().setOccupant(const_cast<Piece*>(&piece));
        }
        catch (const out_of_range&) {
            cout << "Erreur inattendue lors du placement de la pièce" << endl;
            return false;
        }
    }

    notifyObservers();
    return true;
}

bool Grid::canPlacePiece(const Piece& piece, int baseX, int baseY) const {
    for (const pair<int, int>& coord : piece.getCoordinates()) {
        int x = baseX + coord.first;
        int y = baseY + coord.second;

        if (x < 0 || x >= width || y < 0 || y >= height) {
            return false;

        }

        try {
            const GridRow& row = static_cast<const GridRow&>(*rows[y]);
            const CellComponent& cell = row.getCellAt(x);
            if (cell.getCell().isOccupied()) {
                return false;
            }
        }
        catch (const out_of_range&) {
            return false;
        }
    }
    return true;
}

void Grid::clear() {
    for (unique_ptr<GridComponent>& row : rows) {
        try {
            GridRow& gridRow = static_cast<GridRow&>(*row);
            const vector<unique_ptr<GridComponent>>& cells = gridRow.getCells();
            for (size_t i = 0; i < cells.size(); ++i) {
                CellComponent& cellComp = static_cast<CellComponent&>(*cells[i]);
                cellComp.getCell().removeOccupant();
            }
        }
        catch (const out_of_range&) {
            continue;
        }
    }
    notifyObservers();
}

void Grid::afficher(ostream& os) const {
    for (const unique_ptr<GridComponent>& row : rows) {
        try {
            const GridRow& gridRow = static_cast<const GridRow&>(*row);
            for (const unique_ptr<GridComponent>& cell : gridRow.getCells()) {
                const CellComponent& cellComp = static_cast<const CellComponent&>(*cell);
                os << (cellComp.getCell().isOccupied() ? "X" : ".");
            }
            os << endl;
        }
        catch (const out_of_range&) {
            os << "Erreur lors de l'affichage d'une ligne" << endl;
        }
    }
}

Grid::~Grid() {
    // Nettoyer les lignes
    rows.clear();
}