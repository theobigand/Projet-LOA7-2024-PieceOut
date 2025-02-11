CXX = g++
CXXFLAGS = --std=c++11 -Wall -Iinclude
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

SRC_DIR = src
OBJ_DIR = src/obj
MODEL_DIR = $(SRC_DIR)/model
VIEW_DIR = $(SRC_DIR)/view

# Création des répertoires d'objets si nécessaire
$(shell mkdir -p $(OBJ_DIR)/model $(OBJ_DIR)/view)

# Sources du modèle
MODEL_SRCS = \
	$(MODEL_DIR)/Grid.cpp \
	$(MODEL_DIR)/Cell.cpp \
	$(MODEL_DIR)/Observer.cpp \
	$(MODEL_DIR)/PieceConcrete.cpp \
	$(MODEL_DIR)/PieceOperateur.cpp \
	$(MODEL_DIR)/OperateurDeplacement.cpp \
	$(MODEL_DIR)/OperateurRotation.cpp \
	$(MODEL_DIR)/OperateurSymetrie.cpp \
	$(MODEL_DIR)/Model.cpp

# Sources de la vue
VIEW_SRCS = \
	$(VIEW_DIR)/SFMLView.cpp

# Main GUI
MAIN_GUI = $(SRC_DIR)/mainGUI.cpp

# Objets
MODEL_OBJS = $(MODEL_SRCS:$(MODEL_DIR)/%.cpp=$(OBJ_DIR)/model/%.o)
VIEW_OBJS = $(VIEW_SRCS:$(VIEW_DIR)/%.cpp=$(OBJ_DIR)/view/%.o)
MAIN_GUI_OBJ = $(MAIN_GUI:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Exécutable
TARGET = gui

# Règle par défaut
all: $(TARGET)

# Compilation de l'exécutable GUI
$(TARGET): $(MODEL_OBJS) $(VIEW_OBJS) $(MAIN_GUI_OBJ)
	@echo "Version GUI SFML compilée avec succès."
	@$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compilation des fichiers sources du modèle
$(OBJ_DIR)/model/%.o: $(MODEL_DIR)/%.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilation des fichiers sources de la vue
$(OBJ_DIR)/view/%.o: $(VIEW_DIR)/%.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Compilation du main GUI
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Nettoyage
clean:
	rm -rf $(OBJ_DIR)/* $(TARGET)


.PHONY: all clean