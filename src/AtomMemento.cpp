#include "AtomMemento.h"
#include <iostream>

AtomMemento::AtomMemento(std::string state) : state(std::move(state)) {}

void AtomMemento::restore(std::string& stateToRestore) {
    stateToRestore = state;
}

void AtomMemento::printHistory() const {
    std::cout << state;
}