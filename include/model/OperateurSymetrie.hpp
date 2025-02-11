#ifndef OPERATEURSYMETRIE_HPP
#define OPERATEURSYMETRIE_HPP

#include "PieceOperateur.hpp"
#include <stdexcept>

enum class OrientationSymetrie {
    HORIZONTALE,
    VERTICALE
};

class OperateurSymetrie : public PieceOperateur {
private:
    OrientationSymetrie sens;
    pair<int, int> findSymetryCenter(const vector<pair<int, int>>& coords) const;

public:
    explicit OperateurSymetrie(Piece& source, const pair<int, int>& position, OrientationSymetrie s);
    virtual ~OperateurSymetrie() = default;

    // Empêcher la copie et le déplacement
    OperateurSymetrie(const OperateurSymetrie&) = delete;
    OperateurSymetrie& operator=(const OperateurSymetrie&) = delete;
    OperateurSymetrie(OperateurSymetrie&&) = delete;
    OperateurSymetrie& operator=(OperateurSymetrie&&) = delete;

    virtual void accept(const PieceOperateur& v) const;
    virtual void mapPosition(pair<int, int>& pos) const;
    virtual void visit(PieceConcrete& p) const;
    virtual void visit(OperateurDeplacement& op) const;
    virtual void visit(OperateurRotation& op) const;
    virtual void visit(OperateurSymetrie& op) const;
    virtual void afficher(ostream& os) const;

    // Accesseur
    OrientationSymetrie getSens() const { return sens; }
};

#endif
