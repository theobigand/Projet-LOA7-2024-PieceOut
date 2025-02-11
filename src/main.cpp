#include "model/Model.hpp"
#include "view/ConsoleView.hpp"

int main() {
    // Initialisation du modèle
    Model model(8, 8);
    
    // Création d'une pièce en forme de L
    vector<pair<int, int>> coords_L = {{0,0}, {0,1}, {0,2}, {1,2}};
    model.ajouterPiece(coords_L);
    
    // Création d'une pièce en forme de T
    vector<pair<int, int>> coords_T = {{0,0}, {1,0}, {2,0}, {1,1}};
    model.ajouterPiece(coords_T);
    
    // Définir la zone d'arrivée (coin opposé de la grille)
    model.setZoneArrivee(7, 7);
    
    // Définir la pièce L comme celle à déplacer vers la zone d'arrivée
    model.setPieceADeplacer(0);
    
    // Création et lancement de la vue console
    ConsoleView view(model);
    view.run();
    
    return 0;
}
