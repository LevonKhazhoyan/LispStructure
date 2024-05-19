#ifndef LISPSTRUCTURE_ATOM_MEMENTO_H
#define LISPSTRUCTURE_ATOM_MEMENTO_H

#include "Memento.h"
#include <string>

class AtomMemento : public Memento {
    std::string state;
public:
    explicit AtomMemento(std::string state);
    void restore(std::string& stateToRestore);
};

#endif // LISPSTRUCTURE_ATOM_MEMENTO_H