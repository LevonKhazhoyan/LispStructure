#include "ListMemento.h"

ListMemento::ListMemento(const std::vector<std::shared_ptr<LispElement>>& state) : state(state) {}

void ListMemento::restore(std::vector<std::shared_ptr<LispElement>>& stateToRestore) {
    stateToRestore = state;
}
