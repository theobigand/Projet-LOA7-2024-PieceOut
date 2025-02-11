#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include <vector>
using namespace std;

/************************************************************
 * Pattern Observer
 * Permet de notifier automatiquement les observateurs
 * lorsque l'état de la grille change
 *
 * Utilisé ici pour :
 * - Subject : la grille (notifie des changements)
 * - Observer : la vue (réagit aux changements)
 ************************************************************/

 // Interface Observer
 // Les classes qui veulent observer la grille doivent implémenter cette interface
class Observer {
public:
    virtual ~Observer() = default; //= default : Demande au compilateur de générer le destructeur par défaut C'est équivalent à un destructeur vide {} C'est plus explicite que de ne pas déclarer de destructeur
    virtual void update() = 0;  // Appelé quand le sujet change
};

// Classe Subject (Observable)
// Gère une collection d'observateurs et les notifie des changements
class Subject {
private:
    vector<Observer*> observers;  // Liste des observateurs

public:
    virtual ~Subject() = default;

    // Méthodes de gestion des observateurs
    void addObserver(Observer* observer);     // Ajoute un observateur
    void removeObserver(Observer* observer);  // Retire un observateur
    void removeAllObservers();  // Retire tous les observateurs
    void notifyObservers();                  // Notifie tous les observateurs
};

#endif