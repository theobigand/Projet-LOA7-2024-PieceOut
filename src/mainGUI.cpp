#include <iostream>
#include <SFML/Graphics.hpp>
#include "model/Model.hpp"
#include "model/Observer.hpp"
#include "view/SFMLView.hpp"

using namespace std;
using namespace sf;

void afficherInstructionsNiveau1() {
    cout << "Démarrage du jeu Piece Out - Niveau 1" << endl;
    cout << "Objectif : Déplacer la pièce rouge jusqu'à la zone but en haut !" << endl;
    cout << "Règles :" << endl;
    cout << "- La pièce rouge ne peut se déplacer que verticalement sur son chemin" << endl;
    cout << "- La pièce verte peut être tournée pour libérer le passage" << endl;
    cout << "\nContrôles :" << endl;
    cout << "- Clic gauche : Sélectionner/Déplacer une pièce" << endl;
    cout << "- Flèches Haut/Bas : Déplacer la pièce rouge" << endl;
    cout << "- R : Rotation horaire de la pièce verte" << endl;
    cout << "- E : Rotation anti-horaire de la pièce verte" << endl;
    cout << "- S : Symétrie verticale" << endl;
    cout << "- H : Symétrie horizontale" << endl;
    cout << "- Tab : Changer de pièce active" << endl;
    cout << "- Esc : Quitter" << endl;
}

void configureNiveau1(Model& model) {
    // Création des pièces
    vector<pair<int, int>> coords_L = { {0,0}, {0,1}, {0,2}, {1,1} };  // Forme en L
    model.ajouterPiece(coords_L);

    vector<pair<int, int>> coords_mobile = { {0,0} };  // Pièce mobile
    model.ajouterPiece(coords_mobile);

    // Placement des pièces
    model.setPieceActive(0);
    model.placerPiece(5, 1);

    model.setPieceActive(1);
    model.placerPiece(6, 6);

    // Store initial positions
    model.storeInitialPositions();

    // Configuration du niveau
    model.setZoneArrivee(6, 0);
    model.setPieceADeplacer(1);

    // Configuration des zones autorisées (chemin vertical)
    for (int y = 0; y < 8; y++) {
        model.ajouterZoneAutorisee(6, y);  // Colonne 6 pour le déplacement vertical
    }

    model.ajouterZoneAutorisee(5, 1);
    model.ajouterZoneAutorisee(5, 2);
    model.ajouterZoneAutorisee(5, 3);
    model.ajouterZoneAutorisee(4, 2);
}

void afficherInstructionsNiveau2() {
    cout << "Démarrage du jeu Piece Out - Niveau 2" << endl;
    cout << "Objectif : Déplacer la pièce en forme de P jusqu'à la zone but !" << endl;
    cout << "Règles :" << endl;
    cout << "- La pièce peut se déplacer horizontalement sur les cases marquées" << endl;
    cout << "- La pièce peut effectuer des rotations" << endl;
    cout << "\nContrôles :" << endl;
    cout << "- Clic gauche : Sélectionner/Déplacer une pièce" << endl;
    cout << "- Flèches Gauche/Droite : Déplacer la pièce" << endl;
    cout << "- R : Rotation horaire" << endl;
    cout << "- E : Rotation anti-horaire" << endl;
    cout << "- S : Symétrie verticale" << endl;
    cout << "- H : Symétrie horizontale" << endl;
    cout << "- Esc : Quitter" << endl;
}

void configureNiveau2(Model& model) {
    // Création de la pièce selon le schéma ASCII
    /*
    0p0
    ppp
    */
    vector<pair<int, int>> coords_piece = {
        {1,0},      // p du haut
        {0,1}, {1,1}, {2,1}  // ppp du bas
    };
    model.ajouterPiece(coords_piece);

    // Placement de la pièce à sa position initiale
    model.setPieceActive(0);
    model.placerPiece(0, 4);  // Position selon le schéma ASCII (le ppp commence à x=0)
    model.setPieceADeplacer(0);

    // Store initial positions
    model.storeInitialPositions();

    // Configuration des zones but selon le schéma ASCII
    /*
    0rrr0
    bbbrr  <- zones but en forme de L inversé
    0b0rr
    */
    model.ajouterZoneArrivee(0, 1);  // Premier b (à gauche)
    model.ajouterZoneArrivee(1, 1);  // Deuxième b
    model.ajouterZoneArrivee(2, 1);  // Troisième b
    model.ajouterZoneArrivee(1, 2);  // b du bas

    // Configuration des zones autorisées selon le schéma ASCII
    /*
    0rrr0
    rrrrr
    0r0rr
    000rr
    0r0rr
    rrrrr
    0rrr0
    */
    // Ajouter toutes les zones autorisées pour chaque ligne
    for (int x = 0; x < 7; x++) {
        // Première ligne : 0rrr0
        if (x >= 1 && x <= 3) model.ajouterZoneAutorisee(x, 0);

        // Deuxième ligne : rrrrr
        if (x <= 4) model.ajouterZoneAutorisee(x, 1);

        // Troisième ligne : 0r0rr
        if (x == 1 || (x >= 3 && x <= 4)) model.ajouterZoneAutorisee(x, 2);

        // Quatrième ligne : 000rr
        if (x >= 3 && x <= 4) model.ajouterZoneAutorisee(x, 3);

        // Cinquième ligne : 0r0rr
        if (x == 1 || (x >= 3 && x <= 4)) model.ajouterZoneAutorisee(x, 4);

        // Sixième ligne : rrrrr
        if (x <= 4) model.ajouterZoneAutorisee(x, 5);

        // Dernière ligne : 0rrr0
        if (x >= 1 && x <= 3) model.ajouterZoneAutorisee(x, 6);
    }
}

void afficherInstructionsNiveau3() {
    cout << "Démarrage du jeu Piece Out - Niveau 3" << endl;
    cout << "Objectif : Déplacer la pièce de la zone rouge jusqu'à la zone rose en passant par le labyrinthe !" << endl;
    cout << "Règles :" << endl;
    cout << "- La pièce peut se déplacer sur les cases blanches" << endl;
    cout << "- La zone violette permet de faire des rotations" << endl;
    cout << "\nContrôles :" << endl;
    cout << "- Clic gauche : Sélectionner/Déplacer la pièce" << endl;
    cout << "- Flèches : Déplacer la pièce" << endl;
    cout << "- R : Rotation horaire (dans la zone violette)" << endl;
    cout << "- E : Rotation anti-horaire (dans la zone violette)" << endl;
    cout << "- S : Symétrie verticale" << endl;
    cout << "- H : Symétrie horizontale" << endl;
    cout << "- Esc : Quitter" << endl;
}

void configureNiveau3(Model& model) {
    // Création de la pièce à déplacer (forme horizontale)
    vector<pair<int, int>> coords_piece = {
        {0,0}  // Pièce simple 1x1
    };
    model.ajouterPiece(coords_piece);

    // Création de la pièce en L (obstacle)
    vector<pair<int, int>> coords_L = {
        {0,0}, {0,1}, {0,2},  // Partie verticale du L (3 cases)
        {1,2}                 // Partie horizontale du L
    };
    model.ajouterPiece(coords_L);

    // Placement de la pièce à déplacer
    model.setPieceActive(0);
    model.placerPiece(0, 5);  // Position du 'p' dans le schéma
    model.setPieceADeplacer(0);

    // Placement de la pièce en L
    model.setPieceActive(1);
    model.placerPiece(2, 3);  // Position initiale du L

    // Store initial positions
    model.storeInitialPositions();

    // Configuration des zones autorisées selon le schéma ASCII
    /*
    000b0
    000r0
    00rrr
    0rarr  <- 'a' représente la pièce en L
    00ar0
    praa0
    000r0
    */

    // Configuration de la zone but
    model.ajouterZoneArrivee(3, 0);  // Position du 'b'

    // Ligne 1 : 000r0
    model.ajouterZoneAutorisee(3, 0);  // Ajout de la zone but comme zone autorisée
    model.ajouterZoneAutorisee(3, 1);

    // Ligne 2 : 00rrr
    model.ajouterZoneAutorisee(2, 2);
    model.ajouterZoneAutorisee(3, 2);
    model.ajouterZoneAutorisee(4, 2);

    // Ligne 3 : 0rarr
    model.ajouterZoneAutorisee(1, 3);
    model.ajouterZoneAutorisee(2, 3);  // Zone pour la pièce en L
    model.ajouterZoneAutorisee(3, 3);
    model.ajouterZoneAutorisee(4, 3);

    // Ligne 4 : 00ar0
    model.ajouterZoneAutorisee(2, 4);  // Zone pour la pièce en L
    model.ajouterZoneAutorisee(3, 4);

    // Ligne 5 : praa0
    model.ajouterZoneAutorisee(0, 5);  // Position initiale de la pièce à déplacer
    model.ajouterZoneAutorisee(1, 5);
    model.ajouterZoneAutorisee(2, 5);  // Zone pour la pièce en L
    model.ajouterZoneAutorisee(3, 5);

    // Ligne 6 : 000r0
    model.ajouterZoneAutorisee(3, 6);

    // Ajout des zones autorisées pour le mouvement vertical de la pièce en L
    // La pièce en L occupe 4 cases (3 verticales + 1 horizontale)
    // Il faut autoriser toutes les positions possibles
    for (int y = 3; y <= 5; y++) {
        model.ajouterZoneAutorisee(2, y);     // Pour la partie verticale du L
        model.ajouterZoneAutorisee(3, y);     // Pour la partie horizontale du L
        if (y > 0) {
            model.ajouterZoneAutorisee(2, y - 1);  // Pour la case du haut lors du déplacement
            model.ajouterZoneAutorisee(3, y - 1);  // Pour la partie horizontale lors du déplacement
        }
        if (y < 6) {
            model.ajouterZoneAutorisee(2, y + 1);  // Pour la case du bas lors du déplacement
            model.ajouterZoneAutorisee(3, y + 1);  // Pour la partie horizontale lors du déplacement
        }
    }
}

void runLevel(int level) {
    Model model(7, 7);
    string levelTitle = "Piece Out - Niveau " + to_string(level);
    if (Model::isLevelCompleted(level)) {
        levelTitle += " (Complete)";
    }

    switch (level) {
    case 1:
        configureNiveau1(model);
        afficherInstructionsNiveau1();
        break;
    case 2:
        configureNiveau2(model);
        afficherInstructionsNiveau2();
        break;
    case 3:
        configureNiveau3(model);
        afficherInstructionsNiveau3();
        break;
    }

    {
        SFMLView view(model);
        view.setWindowTitle(levelTitle);
        view.run();

        if (model.isPartieGagnee()) {
            Model::setLevelCompleted(level);
        }
    }  // La vue est détruite ici, ce qui ferme la fenêtre
}

int main() {
    try {
        bool running = true;
        while (running) {
            // Create menu window
            RenderWindow menuWindow(VideoMode(800, 600), "Piece Out - Menu");
            menuWindow.setFramerateLimit(60);

            // Load font
            Font font;
            const vector<string> fontPaths = {
                "resources/arial.ttf",
                "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
                "/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf"
            };

            bool fontLoaded = false;
            for (const string& path : fontPaths) {
                if (font.loadFromFile(path)) {
                    fontLoaded = true;
                    break;
                }
            }

            if (!fontLoaded) {
                throw runtime_error("Impossible de charger une police");
            }

            // Create title
            Text title("Piece Out", font, 48);
            title.setFillColor(Color::White);
            title.setPosition(300, 50);

            // Create menu options with completion status
            vector<Text> menuOptions;
            vector<string> optionStrings = {
                string("Niveau 1") + (Model::isLevelCompleted(1) ? " *" : ""),
                string("Niveau 2") + (Model::isLevelCompleted(2) ? " *" : ""),
                string("Niveau 3") + (Model::isLevelCompleted(3) ? " *" : ""),
                "Quitter"
            };

            for (size_t i = 0; i < optionStrings.size(); ++i) {
                Text option(optionStrings[i], font, 36);
                option.setFillColor(Color::White);
                option.setPosition(350, 200 + i * 80);
                menuOptions.push_back(option);
            }

            while (menuWindow.isOpen()) {
                Event event;
                while (menuWindow.pollEvent(event)) {
                    if (event.type == Event::Closed) {
                        menuWindow.close();
                        running = false;
                    }

                    if (event.type == Event::MouseButtonPressed) {
                        Vector2i mousePos = Mouse::getPosition(menuWindow);

                        // Check if any option was clicked
                        for (size_t i = 0; i < menuOptions.size(); ++i) {
                            if (menuOptions[i].getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                                menuWindow.close();
                                if (i < 3) {  // Level options
                                    runLevel(i + 1);
                                } else {  // Quit option
                                    running = false;
                                }
                            }
                        }
                    }

                    // Highlight option under mouse
                    Vector2i mousePos = Mouse::getPosition(menuWindow);
                    for (Text& option : menuOptions) {
                        if (option.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                            option.setFillColor(Color::Yellow);
                        } else {
                            option.setFillColor(Color::White);
                        }
                    }
                }

                menuWindow.clear(Color(50, 50, 50));
                menuWindow.draw(title);
                for (const Text& option : menuOptions) {
                    menuWindow.draw(option);
                }
                menuWindow.display();
            }

            // Nettoyer les ressources du menu
            menuOptions.clear();
        }
        return EXIT_SUCCESS;
    }
    catch (const exception& e) {
        cerr << "Erreur : " << e.what() << endl;
        return EXIT_FAILURE;
    }
}
