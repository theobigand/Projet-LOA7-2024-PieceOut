#pragma once

#include "model/Model.hpp"
#include "model/Observer.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <memory>

using namespace std;
using namespace sf;

class SFMLView : public Observer {
public:
    SFMLView(Model& m);
    ~SFMLView();

    void run();
    void update() override;
    bool shouldReturnToMenu() const { return returnToMenu; }
    void setWindowTitle(const string& title) { window.setTitle(title); }

private:
    // Constantes de l'interface
    static const int TILE_SIZE = 40;
    static const int MARGIN_TOP = 50;
    static const int MARGIN_LEFT = 50;
    static const int MARGIN_RIGHT = 200;
    static const int MARGIN_BOTTOM = 50;
    static const int BUTTON_WIDTH = 150;
    static const int BUTTON_HEIGHT = 30;
    static const int BUTTON_SPACING = 10;

    // Dimensions de la grille
    int VISIBLE_ROWS;
    int VISIBLE_COLS;
    int NB_COL;
    int NB_LIGNE;

    // Références et objets principaux
    Model& model;
    RenderWindow window;
    Font font;

    // Éléments de l'interface
    RectangleShape gridPane;
    VertexArray gridLines;
    vector<RectangleShape> buttons;
    vector<Text> buttonLabels;
    vector<RectangleShape> pieceShapes;     // Formes des pièces

    // État de l'interface
    Vector2i selectedCell;
    bool isDragging;
    size_t draggedPieceIndex;
    Vector2f dragOffset;
    bool victoryDisplayed;
    bool returnToMenu;  // Indicateur de retour au menu

    // Textures et sprites
    map<string, Texture> textures;    // Textures pour les pièces

    // Initialisation
    void loadFonts();
    void initWindow();
    void initGrid();
    void initGridPane();
    void initHorizontalLines(int& lineIndex);
    void initVerticalLines(int& lineIndex);
    void initButtons();
    void loadTextures();
    void initPieceShapes();  // Initialisation des formes des pièces

    // Gestion des événements
    void handleEvents();
    void handleKeyPress(Keyboard::Key key);
    void handleNavigationKeys(Keyboard::Key key);
    void handleTransformationKeys(Keyboard::Key key);
    void handlePieceMovement(Keyboard::Key key);
    void handleMouseClick(const Vector2i& mousePos);
    void handleMouseMove(const Vector2i& mousePos);
    void handleMouseRelease(const Vector2i& mousePos);
    void handleButtonClick(int buttonIndex);
    bool handleButtonClicks(const Vector2i& mousePos);
    bool isPieceAtPosition(size_t pieceIndex, const Vector2i& gridPos);
    bool handlePieceSelection(const Vector2i& gridPos, const Vector2i& mousePos);

    // Dessin
    void drawGrid();
    void drawPieces();
    void drawButtons();
    void drawUI();
    void drawActivePieceInfo();
    void drawControlsInfo();
    void drawVictoryMessage();
    void drawVictoryOverlay();
    void createVictoryText(Text& victoryText);
    void centerText(Text& text);
    void handleVictoryCondition();
    void drawFrame();
    void updatePieceShapes();  // Mise à jour des formes des pièces
    void createPieceShape(const Vector2i& gridPos, const Color& fillColor, const Color& outlineColor, int outlineThickness);
    void updateStaticPieces();
    void updateDraggedPiece();

    // Utilitaires
    Vector2i windowToGrid(const Vector2i& windowPos) const;
    Vector2f gridToWindow(const Vector2i& gridPos) const;
    bool isInsideGrid(const Vector2i& pos) const;
    bool isVisibleInGrid(const Vector2i& pos) const;
    bool isInsideButton(const Vector2i& pos, int buttonIndex) const;
    bool isValidMove(const pair<int, int>& newPos);
    bool isPathClear(const pair<int, int>& start, const pair<int, int>& end);
    bool isPositionOccupied(int x, int y);
    bool isVerticalPathClear(const pair<int, int>& start, const pair<int, int>& end);
    bool isHorizontalPathClear(const pair<int, int>& start, const pair<int, int>& end);

    // Gestion du drag & drop
    void startDragging(size_t pieceIndex, const Vector2i& mousePos);
    void stopDragging();
    void updateDraggedPiece(const Vector2i& mousePos);
};