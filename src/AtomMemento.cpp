#include "AtomMemento.h"


AtomMemento::AtomMemento(std::string state) : state(std::move(state)) {}

void AtomMemento::restore(std::string& stateToRestore) {
    stateToRestore = state;
}