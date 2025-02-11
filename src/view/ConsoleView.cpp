#include "view/ConsoleView.hpp"
#include <sstream>
#include <limits>

ConsoleView::ConsoleView(Model& m) : model(m) {
    // S'enregistrer comme observateur du modèle
    model.addObserver(this);
}

void ConsoleView::afficherGrilleNumerotee() const {
    cout << "\n  ";
    // Affichage des numéros de colonne
    for (int i = 0; i < 8; i++) {
        cout << i << " ";
    }
    cout << "\n  ";
    for (int i = 0; i < 8; i++) {
        cout << "--";
    }
    cout << endl;

    // Affichage de la grille avec numéros de ligne
    string gridStr;
    stringstream ss;
    model.getGrille().afficher(ss);
    gridStr = ss.str();

    // Position de la zone d'arrivée
    pair<int, int> zone = model.getZoneArrivee();

    // Récupérer les informations sur les pièces
    size_t pieceADeplacer = model.getPieceADeplacer();

    int ligne = 0;
    size_t pos = 0;
    while ((pos = gridStr.find('\n')) != string::npos) {
        cout << ligne << "|";
        string ligneCourante = gridStr.substr(0, pos);

        // Parcourir chaque caractère de la ligne
        for (size_t i = 0; i < ligneCourante.length(); ++i) {
            if (static_cast<int>(i) == zone.first && ligne == zone.second) {
                // Case d'arrivée
                cout << ROUGE << (ligneCourante[i] == 'X' ? 'X' : 'O') << RESET;
            } else if (ligneCourante[i] == 'X') {
                // Trouver quelle pièce occupe cette position
                bool estPieceADeplacer = false;
                for (size_t p = 0; p < model.getNbPieces(); ++p) {
                    if (model.isPiecePosee(p)) {
                        pair<int, int> pos = model.getPosition(p);
                        const vector<pair<int, int>>& coords = model.getPieceCoords(p);
                        for (const pair<int, int>& coord : coords) {
                            int px = pos.first + coord.first;
                            int py = pos.second + coord.second;
                            if (px == static_cast<int>(i) && py == ligne) {
                                if (p == pieceADeplacer) {
                                    estPieceADeplacer = true;
                                }
                                break;
                            }
                        }
                    }
                }
                // Afficher en vert si c'est la pièce à déplacer, en jaune sinon
                cout << (estPieceADeplacer ? VERT : JAUNE) << 'X' << RESET;
            } else {
                cout << ligneCourante[i];  // Case vide
            }
        }
        cout << endl;

        gridStr.erase(0, pos + 1);
        ligne++;
    }

    // Afficher la position de la zone d'arrivée
    cout << "\nZone d'arrivée : " << ROUGE << "(" << zone.first << "," << zone.second << ")" << RESET << endl;
}

void ConsoleView::afficherPieces() const {
    try {
        cout << "\nPièces disponibles :" << endl;
        for (size_t i = 0; i < model.getNbPieces(); ++i) {
            // Choisir la couleur en fonction du type de pièce
            string couleur = (i == model.getPieceADeplacer() ? VERT : JAUNE);

            cout << couleur << "Pièce " << i << " : ";
            if (i == model.getPieceActiveIndex()) {
                cout << "(Active) ";
            }
            if (i == model.getPieceADeplacer()) {
                cout << "(À déplacer vers la zone d'arrivée) ";
            }

            if (model.isPiecePosee(i)) {
                pair<int, int> pos = model.getPosition(i);
                cout << "Position (" << pos.first << "," << pos.second << ") ";
            } else {
                cout << "Non placée ";
            }

            cout << "Coordonnées : ";
            const vector<pair<int, int>>& coords = model.getPieceCoords(i);
            for (const pair<int, int>& coord : coords) {
                cout << "(" << coord.first << "," << coord.second << ") ";
            }
            cout << RESET << endl;
        }

        if (model.isPartieGagnee()) {
            cout << "\n" << VERT << "🎉 VICTOIRE ! La pièce a atteint la zone d'arrivée ! 🎉" << RESET << endl;
        }
    }
    catch (const runtime_error& e) {
        cout << ROUGE << "Erreur : " << e.what() << RESET << endl;
    }
}

void ConsoleView::afficherMenu() const {
    cout << "\n=== Menu Principal ===" << endl;
    cout << "1. Placer une pièce" << endl;
    cout << "2. Effectuer une rotation" << endl;
    cout << "3. Effectuer une symétrie" << endl;
    cout << "4. Déplacer une pièce" << endl;
    cout << "5. Changer de pièce active" << endl;
    cout << "6. Effacer la grille" << endl;
    cout << "7. Quitter" << endl;
    cout << "\nVotre choix : ";
}

int ConsoleView::lireChoix() const {
    int choix;
    while (!(cin >> choix)) {
        cout << "Entrée invalide. Veuillez entrer un nombre : ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return choix;
}

pair<int, int> ConsoleView::lireCoordonnees() const {
    int x, y;
    cout << "Position X (0-7) : ";
    while (!(cin >> x) || x < 0 || x > 7) {
        cout << "Position X invalide. Entrez un nombre entre 0 et 7 : ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cout << "Position Y (0-7) : ";
    while (!(cin >> y) || y < 0 || y > 7) {
        cout << "Position Y invalide. Entrez un nombre entre 0 et 7 : ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return make_pair(x, y);
}

size_t ConsoleView::lirePieceActive() const {
    size_t index;
    cout << "Numéro de la pièce (0-" << model.getNbPieces() - 1 << ") : ";
    while (!(cin >> index) || index >= model.getNbPieces()) {
        cout << "Numéro invalide. Entrez un nombre entre 0 et " << model.getNbPieces() - 1 << " : ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return index;
}

void ConsoleView::update() {
    // Ne rien faire ici, l'affichage est géré dans la boucle principale
}

void ConsoleView::run() {
    cout << "=== Jeu de manipulation de pièces ===" << endl;
    cout << "Grille 8x8 avec plusieurs pièces" << endl;

    bool continuer = true;
    while (continuer) {
        // Affichage de l'état actuel
        afficherGrilleNumerotee();
        afficherPieces();
        afficherMenu();

        // Traitement du choix utilisateur
        int choix = lireChoix();
        switch (choix) {
        case 1: { // Placement de pièce
            cout << "\n=== Placement de pièce ===" << endl;
            pair<int, int> pos = lireCoordonnees();
            if (!model.placerPiece(pos.first, pos.second)) {
                cout << "⚠️ Placement impossible à cette position" << endl;
            } else {
                cout << "✅ Pièce placée avec succès" << endl;
            }
            break;
        }

        case 2: { // Rotation
            cout << "\n=== Rotation ===" << endl;
            cout << "1. Horaire" << endl;
            cout << "2. Anti-horaire" << endl;
            cout << "Votre choix : ";
            int sens = lireChoix();
            if (sens == 1 || sens == 2) {
                OrientationRotation orientation =
                    (sens == 1) ? OrientationRotation::HORAIRE : OrientationRotation::ANTIHORAIRE;
                if (!model.rotationPiece(orientation)) {
                    cout << "⚠️ La rotation crée une position invalide" << endl;
                } else {
                    cout << "✅ Rotation effectuée" << endl;
                }
            } else {
                cout << "⚠️ Choix invalide" << endl;
            }
            break;
        }

        case 3: { // Symétrie
            cout << "\n=== Symétrie ===" << endl;
            cout << "1. Horizontale" << endl;
            cout << "2. Verticale" << endl;
            cout << "Votre choix : ";
            int sens = lireChoix();
            if (sens == 1 || sens == 2) {
                OrientationSymetrie orientation =
                    (sens == 1) ? OrientationSymetrie::HORIZONTALE : OrientationSymetrie::VERTICALE;
                if (!model.symetriePiece(orientation)) {
                    cout << "⚠️ La symétrie crée une position invalide" << endl;
                } else {
                    cout << "✅ Symétrie effectuée" << endl;
                }
            } else {
                cout << "⚠️ Choix invalide" << endl;
            }
            break;
        }

        case 4: { // Déplacement
            cout << "\n=== Déplacement ===" << endl;
            cout << "1. Nord (↑)" << endl;
            cout << "2. Sud (↓)" << endl;
            cout << "3. Est (→)" << endl;
            cout << "4. Ouest (←)" << endl;
            cout << "Votre choix : ";
            int direction = lireChoix();
            OrientationDeplacement orientation;
            bool choixValide = true;

            switch (direction) {
            case 1: orientation = OrientationDeplacement::NORD; break;
            case 2: orientation = OrientationDeplacement::SUD; break;
            case 3: orientation = OrientationDeplacement::EST; break;
            case 4: orientation = OrientationDeplacement::OUEST; break;
            default: choixValide = false;
            }

            if (choixValide) {
                if (!model.deplacerPiece(orientation)) {
                    cout << "⚠️ Le déplacement crée une position invalide" << endl;
                } else {
                    cout << "✅ Déplacement effectué" << endl;
                }
            } else {
                cout << "⚠️ Direction invalide" << endl;
            }
            break;
        }

        case 5: { // Changer de pièce active
            cout << "\n=== Changement de pièce active ===" << endl;
            size_t index = lirePieceActive();
            model.setPieceActive(index);
            cout << "✅ Pièce " << index << " activée" << endl;
            break;
        }

        case 6: { // Effacement
            model.effacerGrille();
            cout << "✅ Grille effacée" << endl;
            break;
        }

        case 7: { // Quitter
            continuer = false;
            cout << "\nMerci d'avoir joué ! Au revoir." << endl;
            break;
        }

        default:
            cout << "⚠️ Choix invalide. Veuillez choisir entre 1 et 7." << endl;
        }
        cout << "\nAppuyez sur Entrée pour continuer...";
        cin.get();
        cout << "\033[2J\033[H";  // Efface l'écran
    }
}