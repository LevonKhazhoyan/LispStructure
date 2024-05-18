#ifndef LISPSTRUCTURE_LIST_MEMENTO_H
#define LISPSTRUCTURE_LIST_MEMENTO_H

#include "Memento.h"
#include "LispElement.h"
#include <vector>
#include <memory>
#include <iostream>

class ListMemento : public Memento {
    std::vector<std::shared_ptr<LispElement>> state;
public:
    explicit ListMemento(const std::vector<std::shared_ptr<LispElement>>& state);
    void restore(std::vector<std::shared_ptr<LispElement>>& stateToRestore);
    void printHistory() const override;
};

#endif // LISPSTRUCTURE_LIST_MEMENTO_H