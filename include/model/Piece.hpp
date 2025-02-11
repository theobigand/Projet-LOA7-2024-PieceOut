#ifndef PIECE_HPP
#define PIECE_HPP

#include <vector>
#include <utility>
#include <ostream>
using namespace std;
class PieceOperateur;

class Piece {
public:
    virtual ~Piece() {}
    /**
     * @brief Déclenche une transformation sur la pièce
     * 
     * @param coord Coordonnées relatives de la transformation
     * @param origin Pièce d'origine
     */
    virtual void trigger(const pair<int, int>& coord, const Piece& origin) = 0;
    void trigger(const pair<int, int>& coord) { trigger(coord, *this); }
    virtual const vector<pair<int, int>>& getCoordinates() const = 0;
    /**
     * @brief Accepte un visiteur pour appliquer une transformation
     * 
     * @param v Visiteur à accepter
     */
    virtual void accept(const PieceOperateur& v) const = 0;
    virtual void afficher(ostream& os) const = 0;
    friend ostream& operator<<(ostream& os, const Piece& piece) {
        piece.afficher(os);
        return os;
    }
};

#endif
