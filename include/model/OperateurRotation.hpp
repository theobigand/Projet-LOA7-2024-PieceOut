#ifndef OPERATEUR_ROTATION_HPP
#define OPERATEUR_ROTATION_HPP

#include "model/PieceOperateur.hpp"
#include "model/OrientationRotation.hpp"
#include <utility>
#include <stdexcept>

/**
 * @brief Opérateur de rotation pour les pièces
 * 
 * Cette classe implémente la transformation de rotation pour les pièces,
 * en utilisant le pattern Visitor pour appliquer la transformation.
 */
class OperateurRotation : public PieceOperateur {
public:
    /**
     * @brief Constructeur
     * 
     * @param source Pièce source
     * @param position Position relative de l'opérateur
     * @param sens Sens de rotation (HORAIRE ou ANTI_HORAIRE)
     */
    OperateurRotation(Piece& source, const pair<int, int>& position, OrientationRotation sens);

    /**
     * @brief Transforme une position selon la rotation
     * 
     * @param pos Position à transformer
     */
    void mapPosition(pair<int, int>& pos) const override;

    // Méthodes du pattern Visitor
    void visit(PieceConcrete& p) const override;
    void visit(OperateurDeplacement& op) const override;
    void visit(OperateurRotation& op) const override;
    void visit(OperateurSymetrie& op) const override;
    void accept(const PieceOperateur& v) const override;

    /**
     * @brief Affiche l'opérateur
     * 
     * @param os Flux de sortie
     */
    void afficher(ostream& os) const override;

private:
    OrientationRotation sens;  ///< Sens de rotation

    /**
     * @brief Trouve le centre de rotation
     * 
     * @param coords Coordonnées de la pièce
     * @return Point central pour la rotation
     * @throws invalid_argument si les coordonnées sont invalides
     */
    pair<int, int> findRotationCenter(const vector<pair<int, int>>& coords) const;
};

/**
 * @brief Opérateur de flux pour afficher un OperateurRotation
 * 
 * @param os Flux de sortie
 * @param op Opérateur à afficher
 * @return Référence vers le flux
 */
ostream& operator<<(ostream& os, const OperateurRotation& op);

#endif
