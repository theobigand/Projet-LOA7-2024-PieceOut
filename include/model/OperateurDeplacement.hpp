#ifndef OPERATEURDEPLACEMENT_HPP
#define OPERATEURDEPLACEMENT_HPP

#include "PieceOperateur.hpp"

enum class OrientationDeplacement {
    NORD,
    SUD,
    EST,
    OUEST
};

class OperateurDeplacement : public PieceOperateur {
private:
    OrientationDeplacement direction;

public:
    explicit OperateurDeplacement(Piece& source, const pair<int, int>& position, OrientationDeplacement d);
    virtual ~OperateurDeplacement() = default;

    // Empêcher la copie et le déplacement
    OperateurDeplacement(const OperateurDeplacement&) = delete;
    OperateurDeplacement& operator=(const OperateurDeplacement&) = delete;
    OperateurDeplacement(OperateurDeplacement&&) = delete;
    OperateurDeplacement& operator=(OperateurDeplacement&&) = delete;

    virtual void accept(const PieceOperateur& v) const;
    virtual void mapPosition(pair<int, int>& pos) const;
    virtual void visit(PieceConcrete& p) const;
    virtual void visit(OperateurDeplacement& op) const;
    virtual void visit(OperateurRotation& op) const;
    virtual void visit(OperateurSymetrie& op) const;
    virtual void afficher(ostream& os) const;

    // Accesseur
    OrientationDeplacement getDirection() const { return direction; }
};

#endif
