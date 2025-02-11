#ifndef CONSOLEVIEW_HPP
#define CONSOLEVIEW_HPP

#include "model/Model.hpp"
#include "model/Observer.hpp"
#include <iostream>

/************************************************************
 * Classe ConsoleView
 * Gère l'interface utilisateur en mode console
 * 
 * Pattern Observer :
 * - Hérite de Observer pour être notifiée des changements du modèle
 * 
 * Responsabilités :
 * - Afficher l'état du jeu
 * - Gérer les interactions utilisateur
 * - Mettre à jour l'affichage quand le modèle change
 ************************************************************/
class ConsoleView : public Observer {
private:
    Model& model;  // Référence vers le modèle

    // Codes couleur ANSI
    const string RESET = "\033[0m";
    const string ROUGE = "\033[31m";
    const string VERT = "\033[32m";
    const string JAUNE = "\033[33m";

    // Méthodes d'affichage
    void afficherGrilleNumerotee() const;
    void afficherPieces() const;
    void afficherMenu() const;
    
    // Méthodes de lecture
    int lireChoix() const;
    pair<int, int> lireCoordonnees() const;
    size_t lirePieceActive() const;

public:
    ConsoleView(Model& m);
    
    // Méthode principale de la vue
    void run();
    
    // Méthode de l'Observer pattern
    void update() override;
};

#endif 