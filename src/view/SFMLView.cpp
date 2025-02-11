#include "view/SFMLView.hpp"
#include <iostream>

SFMLView::SFMLView(Model& m)
    : model(m),
    gridLines(Lines, (model.getNbColonnes() + 1 + model.getNbLignes() + 1) * 2),
    selectedCell(-1, -1),
    isDragging(false),
    draggedPieceIndex(0),
    dragOffset(0, 0),
    victoryDisplayed(false),
    returnToMenu(false) {

    // Initialiser les dimensions selon le modèle
    VISIBLE_ROWS = model.getNbLignes();
    VISIBLE_COLS = model.getNbColonnes();
    NB_COL = model.getNbColonnes();
    NB_LIGNE = model.getNbLignes();

    model.addObserver(this);
    initWindow();
    loadFonts();
    initGrid();
    initButtons();
    loadTextures();
}

SFMLView::~SFMLView() {
    // Se retirer de la liste des observateurs du modèle avant la destruction
    model.removeObserver(this);

    // Nettoyer les ressources SFML
    window.close();

    // Vider les vecteurs et libérer la mémoire
    pieceShapes.clear();
    buttons.clear();
    buttonLabels.clear();
    textures.clear();
    gridLines.clear();

    // Libérer l'excès de mémoire pour les vecteurs
    pieceShapes.shrink_to_fit();
    buttons.shrink_to_fit();
    buttonLabels.shrink_to_fit();
}

void SFMLView::loadFonts() {
    // Essayer d'abord notre police locale, puis les polices système en secours
    const vector<string> fontPaths = {
        "resources/arial.ttf",  // Police locale
        "/System/Library/Fonts/Helvetica.ttc",  // Police système macOS
        "/Library/Fonts/Arial.ttf",  // Autre emplacement possible sur macOS
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",  // Polices Linux en dernier recours
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
}

void SFMLView::initWindow() {
    unsigned int width = MARGIN_LEFT + MARGIN_RIGHT + VISIBLE_COLS * TILE_SIZE + BUTTON_WIDTH + BUTTON_SPACING;
    unsigned int height = MARGIN_TOP + MARGIN_BOTTOM + VISIBLE_ROWS * TILE_SIZE;
    window.create(VideoMode(width, height), "Piece Out - Niveau Puzzle");
    window.setFramerateLimit(60);
}

void SFMLView::initButtons() {
    vector<string> buttonNames = {
        "Rotation Horaire",
        "Rotation Anti-horaire",
        "Symetrie Horizontale",
        "Symetrie Verticale",
        "Piece Suivante",
        "Reinitialiser",
        "Menu Principal"
    };

    float buttonX = MARGIN_LEFT + NB_COL * TILE_SIZE + BUTTON_SPACING;
    float buttonY = MARGIN_TOP;

    for (const string& name : buttonNames) {
        // Créer le bouton
        RectangleShape button(Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
        button.setPosition(buttonX, buttonY);
        button.setFillColor(Color(200, 200, 200));
        button.setOutlineThickness(2);
        button.setOutlineColor(Color::Black);
        buttons.push_back(button);

        // Créer le texte du bouton
        Text label(name, font, 16);
        label.setFillColor(Color::Black);
        // Centrer le texte dans le bouton
        FloatRect textBounds = label.getLocalBounds();
        label.setPosition(
            buttonX + (BUTTON_WIDTH - textBounds.width) / 2,
            buttonY + (BUTTON_HEIGHT - textBounds.height) / 2
        );
        buttonLabels.push_back(label);

        buttonY += BUTTON_HEIGHT + BUTTON_SPACING;
    }
}

void SFMLView::initGridPane() {
    // Initialiser le panneau central
    gridPane.setSize(Vector2f(VISIBLE_COLS * TILE_SIZE, VISIBLE_ROWS * TILE_SIZE));
    gridPane.setPosition(MARGIN_LEFT, MARGIN_TOP);
    gridPane.setFillColor(Color(240, 240, 240));  // Gris très clair
    gridPane.setOutlineThickness(2);
    gridPane.setOutlineColor(Color::Black);
}

void SFMLView::initHorizontalLines(int& lineIndex) {
    // Lignes horizontales
    for (int i = 0; i <= VISIBLE_ROWS; ++i) {
        gridLines[lineIndex].position = Vector2f(MARGIN_LEFT, MARGIN_TOP + i * TILE_SIZE);
        gridLines[lineIndex].color = Color(180, 180, 180);  // Gris moyen
        lineIndex++;
        gridLines[lineIndex].position = Vector2f(MARGIN_LEFT + VISIBLE_COLS * TILE_SIZE, MARGIN_TOP + i * TILE_SIZE);
        gridLines[lineIndex].color = Color(180, 180, 180);
        lineIndex++;
    }
}

void SFMLView::initVerticalLines(int& lineIndex) {
    // Lignes verticales
    for (int i = 0; i <= VISIBLE_COLS; ++i) {
        gridLines[lineIndex].position = Vector2f(MARGIN_LEFT + i * TILE_SIZE, MARGIN_TOP);
        gridLines[lineIndex].color = Color(180, 180, 180);
        lineIndex++;
        gridLines[lineIndex].position = Vector2f(MARGIN_LEFT + i * TILE_SIZE, MARGIN_TOP + VISIBLE_ROWS * TILE_SIZE);
        gridLines[lineIndex].color = Color(180, 180, 180);
        lineIndex++;
    }
}

void SFMLView::initGrid() {
    // Initialiser le panneau central
    initGridPane();

    // Initialiser les lignes de la grille
    gridLines.resize((VISIBLE_COLS + 1 + VISIBLE_ROWS + 1) * 2);
    int lineIndex = 0;

    // Initialiser les lignes horizontales et verticales
    initHorizontalLines(lineIndex);
    initVerticalLines(lineIndex);
}

void SFMLView::loadTextures() {
    // Nous n'utilisons plus de textures, mais des formes colorées
}

void SFMLView::handleVictoryCondition() {
    if (model.isPartieGagnee() && !victoryDisplayed) {
        // Dessiner une dernière frame pour montrer la position finale
        window.clear(Color::White);
        drawGrid();
        drawPieces();
        drawUI();
        window.display();

        // Attendre un court instant pour montrer la position finale
        sleep(milliseconds(200));

        // Afficher le message de victoire
        drawVictoryMessage();
        window.display();

        // Afficher brièvement le message de victoire
        sleep(milliseconds(800));

        victoryDisplayed = true;
        window.close();
    }
}

void SFMLView::drawFrame() {
    window.clear(Color::White);
    drawGrid();
    drawPieces();
    drawUI();
    window.display();
}

void SFMLView::run() {
    while (window.isOpen()) {
        // Gérer tous les événements en attente
        handleEvents();

        // Vérifier la condition de victoire
        handleVictoryCondition();

        // Dessiner la frame courante
        if (window.isOpen()) {  // Vérifier à nouveau car handleVictoryCondition peut fermer la fenêtre
            drawFrame();
        }
    }
}

void SFMLView::handleEvents() {
    Event event;
    while (window.pollEvent(event)) {
        switch (event.type) {
        case Event::Closed:
            window.close();
            break;

        case Event::KeyPressed:
            handleKeyPress(event.key.code);
            break;

        case Event::MouseButtonPressed:
            if (event.mouseButton.button == Mouse::Left) {
                handleMouseClick(Vector2i(event.mouseButton.x, event.mouseButton.y));
            }
            break;

        case Event::MouseButtonReleased:
            if (event.mouseButton.button == Mouse::Left) {
                handleMouseRelease(Vector2i(event.mouseButton.x, event.mouseButton.y));
            }
            break;

        case Event::MouseMoved:
            handleMouseMove(Vector2i(event.mouseMove.x, event.mouseMove.y));
            break;

        default:
            break;
        }
    }
}

void SFMLView::handleNavigationKeys(Keyboard::Key key) {
    switch (key) {
    case Keyboard::Escape:
        window.close();
        break;

    case Keyboard::M:  // Retour au menu
        window.close();
        returnToMenu = true;
        break;

    case Keyboard::Tab:  // Changer de pièce active
        if (model.getNbPieces() > 0) {
            size_t nextPiece = (model.getPieceActiveIndex() + 1) % model.getNbPieces();
            model.setPieceActive(nextPiece);
        }
        break;

    default:
        // Ignorer les autres touches
        break;
    }
}

void SFMLView::handleTransformationKeys(Keyboard::Key key) {
    switch (key) {
    case Keyboard::R:  // Rotation horaire
        model.rotationPiece(OrientationRotation::HORAIRE);
        break;

    case Keyboard::E:  // Rotation anti-horaire
        model.rotationPiece(OrientationRotation::ANTIHORAIRE);
        break;

    case Keyboard::S:  // Symétrie verticale
        model.symetriePiece(OrientationSymetrie::VERTICALE);
        break;

    case Keyboard::H:  // Symétrie horizontale
        model.symetriePiece(OrientationSymetrie::HORIZONTALE);
        break;

    default:
        // Ignorer les autres touches
        break;
    }
}

void SFMLView::handleKeyPress(Keyboard::Key key) {
    // Gérer les touches de navigation
    handleNavigationKeys(key);

    // Gérer les touches de transformation
    handleTransformationKeys(key);

    // Gérer les touches de déplacement
    switch (key) {
    case Keyboard::Up:
    case Keyboard::Down:
    case Keyboard::Left:
    case Keyboard::Right:
        handlePieceMovement(key);
        break;
    default:
        break;
    }
}

void SFMLView::handlePieceMovement(Keyboard::Key key) {
    // Récupérer la pièce active
    size_t activePiece = model.getPieceActiveIndex();
    if (!model.isPiecePosee(activePiece)) {
        return;  // Si la pièce n'est pas placée, ignorer le mouvement
    }

    pair<int, int> pos = model.getPosition(activePiece);
    pair<int, int> newPos = pos;

    // Calculer la nouvelle position selon la touche
    switch (key) {
    case Keyboard::Up:    newPos.second--; break;
    case Keyboard::Down:  newPos.second++; break;
    case Keyboard::Left:  newPos.first--;  break;
    case Keyboard::Right: newPos.first++;  break;
    default: return;  // Ne devrait jamais arriver grâce au switch du caller
    }

    // Vérifier si la nouvelle position est dans les limites de la grille
    const vector<pair<int, int>>& coords = model.getPieceCoords(activePiece);
    bool validMove = true;

    for (const pair<int, int>& coord : coords) {
        int x = newPos.first + coord.first;
        int y = newPos.second + coord.second;

        // Vérifier les limites de la grille
        if (x < 0 || x >= model.getNbColonnes() || y < 0 || y >= model.getNbLignes()) {
            validMove = false;
            break;
        }

        // Vérifier si la nouvelle position est dans une zone autorisée
        if (!model.estZoneAutorisee(x, y)) {
            validMove = false;
            break;
        }
    }

    // Si le mouvement est valide, mettre à jour la position de la pièce
    if (validMove) {
        model.placerPiece(newPos.first, newPos.second);
    }
}

bool SFMLView::handleButtonClicks(const Vector2i& mousePos) {
    // Vérifier les clics sur les boutons
    for (size_t i = 0; i < buttons.size(); ++i) {
        if (isInsideButton(mousePos, i)) {
            handleButtonClick(i);
            return true;
        }
    }
    return false;
}

bool SFMLView::isPieceAtPosition(size_t pieceIndex, const Vector2i& gridPos) {
    if (!model.isPiecePosee(pieceIndex)) return false;

    const vector<pair<int, int>>& coords = model.getPieceCoords(pieceIndex);
    const pair<int, int>& piecePos = model.getPosition(pieceIndex);

    for (const pair<int, int>& coord : coords) {
        if (piecePos.first + coord.first == gridPos.x &&
            piecePos.second + coord.second == gridPos.y) {
            return true;
        }
    }
    return false;
}

bool SFMLView::handlePieceSelection(const Vector2i& gridPos, const Vector2i& mousePos) {
    for (size_t i = 0; i < model.getNbPieces(); ++i) {
        if (isPieceAtPosition(i, gridPos)) {
            // Si c'est la pièce à déplacer, vérifier qu'on est sur une zone autorisée
            if (i == model.getPieceADeplacer()) {
                if (!model.estZoneAutorisee(gridPos.x, gridPos.y)) {
                    return true;  // Ignorer le clic si hors zone autorisée
                }
                startDragging(i, mousePos);
                return true;
            }
            // Si ce n'est pas la pièce à déplacer, on ne permet que la sélection
            model.setPieceActive(i);
            return true;
        }
    }
    return false;
}

void SFMLView::handleMouseClick(const Vector2i& mousePos) {
    if (handleButtonClicks(mousePos)) return;

    Vector2i gridPos = windowToGrid(mousePos);
    if (isInsideGrid(gridPos) && !isDragging) {
        handlePieceSelection(gridPos, mousePos);
    }
}

void SFMLView::handleMouseMove(const Vector2i& mousePos) {
    if (isDragging) {
        updateDraggedPiece(mousePos);
    }
}

void SFMLView::handleMouseRelease(const Vector2i& mousePos) {
    if (isDragging) {
        Vector2i gridPos = windowToGrid(mousePos);
        if (isInsideGrid(gridPos)) {
            if (draggedPieceIndex == model.getPieceADeplacer()) {
                pair<int, int> currentPos = model.getPosition(draggedPieceIndex);
                pair<int, int> newPos = make_pair(gridPos.x, gridPos.y);

                if (isValidMove(newPos) && isPathClear(currentPos, newPos)) {
                    model.placerPiece(newPos.first, newPos.second);
                } else {
                    model.placerPiece(currentPos.first, currentPos.second);
                }
            } else {
                model.placerPiece(gridPos.x, gridPos.y);
            }
        }
        stopDragging();
    }
}

void SFMLView::handleButtonClick(int buttonIndex) {
    switch (buttonIndex) {
    case 0:  // Rotation horaire
        model.rotationPiece(OrientationRotation::HORAIRE);
        break;
    case 1:  // Rotation anti-horaire
        model.rotationPiece(OrientationRotation::ANTIHORAIRE);
        break;
    case 2:  // Symétrie horizontale
        model.symetriePiece(OrientationSymetrie::HORIZONTALE);
        break;
    case 3:  // Symétrie verticale
        model.symetriePiece(OrientationSymetrie::VERTICALE);
        break;
    case 4:  // Pièce suivante
        if (model.getNbPieces() > 0) {
            size_t nextPiece = (model.getPieceActiveIndex() + 1) % model.getNbPieces();
            model.setPieceActive(nextPiece);
        }
        break;
    case 5:  // Réinitialiser
        model.resetToInitialPositions();
        victoryDisplayed = false;
        break;
    case 6:  // Return to menu
        window.close();
        returnToMenu = true;
        break;
    }
}

void SFMLView::drawGrid() {
    // Dessiner le panneau de la grille
    window.draw(gridPane);

    // Dessiner les lignes de la grille
    window.draw(gridLines);

    // Dessiner les zones autorisées
    const set<pair<int, int>>& zones = model.getZonesAutorisees();
    for (const pair<int, int>& zone : zones) {
        Vector2i pathPos(zone.first, zone.second);
        if (isVisibleInGrid(pathPos)) {
            RectangleShape pathCell(Vector2f(TILE_SIZE - 2, TILE_SIZE - 2));
            pathCell.setPosition(gridToWindow(pathPos));
            pathCell.setFillColor(Color(220, 220, 220));  // Gris clair
            pathCell.setOutlineThickness(1);
            pathCell.setOutlineColor(Color(180, 180, 180));
            window.draw(pathCell);
        }
    }

    // Dessiner toutes les zones d'arrivée
    const set<pair<int, int>>& zonesArrivee = model.getZonesArrivee();
    for (const pair<int, int>& zone : zonesArrivee) {
        Vector2i zonePos(zone.first, zone.second);
        if (isVisibleInGrid(zonePos)) {
            RectangleShape targetCell(Vector2f(TILE_SIZE - 2, TILE_SIZE - 2));
            targetCell.setPosition(gridToWindow(zonePos));
            targetCell.setFillColor(Color(255, 0, 0, 180));  // Rouge plus vif et plus opaque
            targetCell.setOutlineThickness(2);
            targetCell.setOutlineColor(Color(200, 0, 0));    // Contour rouge foncé
            window.draw(targetCell);
        }
    }
}

void SFMLView::drawPieces() {
    // Mettre à jour les formes des pièces
    updatePieceShapes();

    // Dessiner toutes les pièces
    for (const RectangleShape& shape : pieceShapes) {
        window.draw(shape);
    }
}

void SFMLView::initPieceShapes() {
    // Pré-allouer un nombre raisonnable de formes
    pieceShapes.reserve(model.getNbPieces() * 4);  // 4 cases max par pièce
}

void SFMLView::createPieceShape(const Vector2i& gridPos, const Color& fillColor, const Color& outlineColor, int outlineThickness) {
    RectangleShape shape(Vector2f(TILE_SIZE - 2, TILE_SIZE - 2));
    shape.setPosition(gridToWindow(gridPos));
    shape.setFillColor(fillColor);
    shape.setOutlineThickness(outlineThickness);
    shape.setOutlineColor(outlineColor);
    pieceShapes.push_back(shape);
}

void SFMLView::updateStaticPieces() {
    for (size_t i = 0; i < model.getNbPieces(); ++i) {
        if (model.isPiecePosee(i) && (!isDragging || i != draggedPieceIndex)) {
            const vector<pair<int, int>>& coords = model.getPieceCoords(i);
            pair<int, int> pos = model.getPosition(i);

            for (const pair<int, int>& coord : coords) {
                Vector2i gridPos(pos.first + coord.first, pos.second + coord.second);
                if (isVisibleInGrid(gridPos)) {
                    Color fillColor = (i == model.getPieceADeplacer()) ?
                        Color(50, 200, 50) :  // Vert pour la pièce à déplacer
                        Color(200, 50, 50);   // Rouge pour les autres pièces

                    Color outlineColor = (i == model.getPieceActiveIndex()) ?
                        Color(255, 215, 0) :  // Bordure dorée
                        Color::Black;

                    int outlineThickness = (i == model.getPieceActiveIndex()) ? 3 : 1;

                    createPieceShape(gridPos, fillColor, outlineColor, outlineThickness);
                }
            }
        }
    }
}

void SFMLView::updateDraggedPiece() {
    if (!isDragging) return;

    const vector<pair<int, int>>& coords = model.getPieceCoords(draggedPieceIndex);
    Vector2i gridPos = windowToGrid(Mouse::getPosition(window) - Vector2i(dragOffset));

    for (const pair<int, int>& coord : coords) {
        Vector2i piecePos(gridPos.x + coord.first, gridPos.y + coord.second);
        if (isVisibleInGrid(piecePos)) {
            Color fillColor = (draggedPieceIndex == model.getPieceADeplacer()) ?
                Color(50, 200, 50, 128) :  // Vert transparent
                Color(200, 50, 50, 128);   // Rouge transparent

            createPieceShape(piecePos, fillColor, Color(255, 215, 0, 128), 3);
        }
    }
}

void SFMLView::updatePieceShapes() {
    pieceShapes.clear();  // Vider le vecteur
    pieceShapes.reserve(model.getNbPieces() * 4);  // Réserver l'espace nécessaire
    updateStaticPieces();
    updateDraggedPiece();
    pieceShapes.shrink_to_fit();  // Libérer l'excès de mémoire
}

void SFMLView::drawButtons() {
    for (size_t i = 0; i < buttons.size(); ++i) {
        window.draw(buttons[i]);
        window.draw(buttonLabels[i]);
    }
}

void SFMLView::drawActivePieceInfo() {
    size_t activePiece = model.getPieceActiveIndex();
    if (activePiece >= model.getNbPieces()) return;

    Text infoText("Piece active: " + to_string(activePiece), font, 16);
    infoText.setPosition(MARGIN_LEFT + NB_COL * TILE_SIZE + BUTTON_WIDTH + BUTTON_SPACING * 2, MARGIN_TOP);
    infoText.setFillColor(Color::Black);
    window.draw(infoText);
}

void SFMLView::drawControlsInfo() {
    // Afficher les contrôles sur deux lignes
    Text controls1("R: Rotation horaire | E: Rotation anti-horaire | S: Symetrie verticale", font, 16);
    Text controls2("H: Symetrie horizontale | Tab: Piece suivante | M: Menu | Esc: Quitter", font, 16);

    controls1.setPosition(MARGIN_LEFT, MARGIN_TOP + NB_LIGNE * TILE_SIZE + 10);
    controls2.setPosition(MARGIN_LEFT, MARGIN_TOP + NB_LIGNE * TILE_SIZE + 30);

    controls1.setFillColor(Color::Black);
    controls2.setFillColor(Color::Black);

    window.draw(controls1);
    window.draw(controls2);
}

void SFMLView::drawUI() {
    // Dessiner les boutons
    drawButtons();

    // Afficher les informations sur la pièce active
    drawActivePieceInfo();

    // Afficher les contrôles
    drawControlsInfo();
}

void SFMLView::drawVictoryOverlay() {
    // Fond semi-transparent noir
    RectangleShape overlay(Vector2f(window.getSize().x, window.getSize().y));
    overlay.setFillColor(Color(0, 0, 0, 180));  // Plus opaque pour meilleure lisibilité
    window.draw(overlay);
}

void SFMLView::createVictoryText(Text& victoryText) {
    // Message de victoire principal
    victoryText.setString("NIVEAU COMPLETE !");
    victoryText.setFont(font);
    victoryText.setCharacterSize(48);
    victoryText.setStyle(Text::Bold);
    victoryText.setFillColor(Color::Green);
    victoryText.setOutlineThickness(3);
    victoryText.setOutlineColor(Color::Black);
}

void SFMLView::centerText(Text& text) {
    // Centrer le message
    FloatRect textBounds = text.getLocalBounds();
    text.setPosition(
        (window.getSize().x - textBounds.width) / 2,
        (window.getSize().y - textBounds.height) / 2
    );
}

void SFMLView::drawVictoryMessage() {
    drawVictoryOverlay();

    Text victoryText;
    createVictoryText(victoryText);
    centerText(victoryText);
    window.draw(victoryText);
}

Vector2i SFMLView::windowToGrid(const Vector2i& windowPos) const {
    // Ajuster la position pour centrer la vue sur la partie visible
    int offsetX = (NB_COL - VISIBLE_COLS) / 2;
    int offsetY = (NB_LIGNE - VISIBLE_ROWS) / 2;
    return Vector2i(
        (windowPos.x - MARGIN_LEFT) / TILE_SIZE + offsetX,
        (windowPos.y - MARGIN_TOP) / TILE_SIZE + offsetY
    );
}

Vector2f SFMLView::gridToWindow(const Vector2i& gridPos) const {
    // Ajuster la position pour centrer la vue sur la partie visible
    int offsetX = (NB_COL - VISIBLE_COLS) / 2;
    int offsetY = (NB_LIGNE - VISIBLE_ROWS) / 2;
    return Vector2f(
        MARGIN_LEFT + (gridPos.x - offsetX) * TILE_SIZE,
        MARGIN_TOP + (gridPos.y - offsetY) * TILE_SIZE
    );
}

bool SFMLView::isInsideGrid(const Vector2i& pos) const {
    // Vérifier si la position est dans la grille complète
    return pos.x >= 0 && pos.x < NB_COL && pos.y >= 0 && pos.y < NB_LIGNE;
}

bool SFMLView::isVisibleInGrid(const Vector2i& pos) const {
    // Vérifier si la position est dans la partie visible de la grille
    int offsetX = (NB_COL - VISIBLE_COLS) / 2;
    int offsetY = (NB_LIGNE - VISIBLE_ROWS) / 2;
    return pos.x >= offsetX && pos.x < offsetX + VISIBLE_COLS &&
        pos.y >= offsetY && pos.y < offsetY + VISIBLE_ROWS;
}

bool SFMLView::isInsideButton(const Vector2i& pos, int buttonIndex) const {
    if (buttonIndex >= 0 && buttonIndex < static_cast<int>(buttons.size())) {
        return buttons[buttonIndex].getGlobalBounds().contains(Vector2f(pos));
    }
    return false;
}

void SFMLView::startDragging(size_t pieceIndex, const Vector2i& mousePos) {
    isDragging = true;
    draggedPieceIndex = pieceIndex;
    model.setPieceActive(pieceIndex);

    // Calculer l'offset pour le drag & drop
    pair<int, int> piecePos = model.getPosition(pieceIndex);
    Vector2f windowPos = gridToWindow(Vector2i(piecePos.first, piecePos.second));
    dragOffset = Vector2f(mousePos - Vector2i(windowPos));
}

void SFMLView::stopDragging() {
    isDragging = false;
}

void SFMLView::updateDraggedPiece(const Vector2i& mousePos) {
    // La mise à jour visuelle est gérée dans drawPieces()
}

void SFMLView::update() {
    // La mise à jour sera faite à la prochaine frame
}

bool SFMLView::isPositionOccupied(int x, int y) {
    // Vérifier si une pièce (autre que celle à déplacer) occupe la position
    for (size_t i = 0; i < model.getNbPieces(); ++i) {
        if (i != model.getPieceADeplacer() && model.isPiecePosee(i)) {
            const vector<pair<int, int>>& coords = model.getPieceCoords(i);
            pair<int, int> pos = model.getPosition(i);
            for (const pair<int, int>& coord : coords) {
                if (pos.first + coord.first == x &&
                    pos.second + coord.second == y) {
                    return true;  // Position occupée
                }
            }
        }
    }
    return false;
}

bool SFMLView::isVerticalPathClear(const pair<int, int>& start, const pair<int, int>& end) {
    int step = (end.second > start.second) ? 1 : -1;
    for (int y = start.second + step; (step > 0) ? (y <= end.second) : (y >= end.second); y += step) {
        if (isPositionOccupied(start.first, y)) {
            return false;  // Chemin bloqué
        }
    }
    return true;
}

bool SFMLView::isHorizontalPathClear(const pair<int, int>& start, const pair<int, int>& end) {
    int step = (end.first > start.first) ? 1 : -1;
    for (int x = start.first + step; (step > 0) ? (x <= end.first) : (x >= end.first); x += step) {
        if (isPositionOccupied(x, start.second)) {
            return false;  // Chemin bloqué
        }
    }
    return true;
}

bool SFMLView::isPathClear(const pair<int, int>& start, const pair<int, int>& end) {
    // Pour un déplacement vertical
    if (start.first == end.first) {
        return isVerticalPathClear(start, end);
    }
    // Pour un déplacement horizontal
    else if (start.second == end.second) {
        return isHorizontalPathClear(start, end);
    }
    // Pour un déplacement diagonal ou autre, on autorise si les deux cases sont libres
    else {
        return isPathClear(start, make_pair(start.first, end.second)) &&
            isPathClear(make_pair(start.first, end.second), end);
    }
    return true;
}

bool SFMLView::isValidMove(const pair<int, int>& newPos) {
    const vector<pair<int, int>>& coords = model.getPieceCoords(model.getPieceADeplacer());

    for (const pair<int, int>& coord : coords) {
        int x = newPos.first + coord.first;
        int y = newPos.second + coord.second;

        if (x < 0 || x >= model.getNbColonnes() || y < 0 || y >= model.getNbLignes()) {
            return false;
        }

        if (!model.estZoneAutorisee(x, y)) {
            return false;
        }
    }

    pair<int, int> currentPos = model.getPosition(model.getPieceADeplacer());
    return isPathClear(currentPos, newPos);
}