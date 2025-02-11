#ifndef PIECEOPERATEUR_HPP
#define PIECEOPERATEUR_HPP

#include "Piece.hpp"
#include <utility>
#include <vector>

class PieceConcrete;
class OperateurDeplacement;
class OperateurRotation;
class OperateurSymetrie;

/**
 * @brief Classe abstraite représentant un opérateur de transformation sur une pièce
 * 
 * Cette classe implémente le pattern Decorator pour ajouter des comportements aux pièces
 * et le pattern Visitor pour permettre différentes transformations.
 * Les transformations sont appliquées de manière récursive sur la structure composite.
 * 
 * @note Les opérations de copie sont désactivées pour éviter les problèmes de propriété
 * @note La classe utilise des références pour éviter les fuites mémoire
 */
class PieceOperateur : public Piece {
    friend class OperateurDeplacement;
    friend class OperateurRotation;
    friend class OperateurSymetrie;

protected:
    Piece& source;  // La pièce sur laquelle l'opérateur agit (référence pour éviter les fuites mémoire)
    pair<int, int> position;  // Position relative de l'opérateur dans la grille

public:
    /**
     * @brief Constructeur de PieceOperateur
     * @param source Référence vers la pièce à transformer
     * @param position Position relative de l'opérateur
     * @note Le constructeur est explicite pour éviter les conversions implicites
     */
    explicit PieceOperateur(Piece& source, const pair<int, int>& position);

    /**
     * @brief Destructeur virtuel
     * Nécessaire car la classe est utilisée comme classe de base
     */
    virtual ~PieceOperateur() = default;

    // Empêcher la copie et le déplacement pour éviter les problèmes de propriété
    PieceOperateur(const PieceOperateur&) = delete;
    PieceOperateur& operator=(const PieceOperateur&) = delete;
    PieceOperateur(PieceOperateur&&) = delete;
    PieceOperateur& operator=(PieceOperateur&&) = delete;

    /**
     * @brief Obtient les coordonnées de la pièce
     * @return Référence constante vers le vecteur de coordonnées
     */
    virtual const vector<pair<int, int>>& getCoordinates() const;

    /**
     * @brief Déclenche une transformation si la position correspond
     * @param relativePos Position relative à vérifier
     * @param origin Pièce d'origine
     */
    virtual void trigger(const pair<int, int>& relativePos, const Piece& origin);

    /**
     * @brief Transforme une position selon l'opérateur
     * @param pos Position à transformer (modifiée par la méthode)
     */
    virtual void mapPosition(pair<int, int>& pos) const = 0;

    /**
     * @brief Pattern Visitor : méthodes pour visiter les différents types de pièces
     * Chaque méthode visit applique la transformation spécifique à un type de pièce
     * @note Les méthodes sont const car elles ne modifient pas l'opérateur lui-même
     */
    virtual void visit(PieceConcrete& p) const = 0;
    virtual void visit(OperateurDeplacement& op) const = 0;
    virtual void visit(OperateurRotation& op) const = 0;
    virtual void visit(OperateurSymetrie& op) const = 0;
    virtual void accept(const PieceOperateur& v) const = 0;

    // Accesseurs sécurisés
    /**
     * @brief Obtient une référence constante vers la pièce source
     */
    const Piece& getSource() const { return source; }

    /**
     * @brief Obtient une référence constante vers la position
     */
    const pair<int, int>& getPosition() const { return position; }

protected:
    /**
     * @brief Obtient une référence modifiable vers la source
     * @note Méthode protégée pour contrôler l'accès en écriture
     */
    Piece& getSourceMutable() { return source; }

    /**
     * @brief Obtient une référence modifiable vers la position
     * @note Méthode protégée pour contrôler l'accès en écriture
     */
    pair<int, int>& getPositionMutable() { return position; }
};

#endif
