#include "model/Observer.hpp"
#include <algorithm>

void Subject::addObserver(Observer* observer) {
    observers.push_back(observer);
}

void Subject::removeObserver(Observer* observer) {
    observers.erase(
        remove(observers.begin(), observers.end(), observer),
        observers.end()
    );
}

void Subject::removeAllObservers() {
    observers.clear();
    observers.shrink_to_fit();
}

void Subject::notifyObservers() {
    for (Observer* observer : observers) {
        observer->update();
    }
}