#ifndef PIECECONCRETE_HPP
#define PIECECONCRETE_HPP

#include "Piece.hpp"
#include <vector>

class PieceOperateur;
class Model;  // Forward declaration
class OperateurDeplacement;  // Forward declaration
class OperateurRotation;     // Forward declaration
class OperateurSymetrie;     // Forward declaration

class PieceConcrete : public Piece {
    friend class Model;  // Allow Model to access private members
    friend class OperateurDeplacement;  // Allow operators to access private members
    friend class OperateurRotation;
    friend class OperateurSymetrie;

public:
    PieceConcrete(const vector<pair<int, int>>& coords);
    virtual const vector<pair<int, int>>& getCoordinates() const override;
    /**
     * @brief Déclenche une transformation sur la pièce
     *
     * @param coord Coordonnées relatives de la transformation
     * @param origin Pièce d'origine
     */
    virtual void trigger(const pair<int, int>& coord, const Piece& origin) override;
    virtual void accept(const PieceOperateur& v) const override;
    void afficher(ostream& os) const override;

private:
    vector<pair<int, int>> coordinates;
};

#endif
