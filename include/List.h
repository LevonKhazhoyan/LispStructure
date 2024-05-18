#ifndef LISPSTRUCTURE_LIST_H
#define LISPSTRUCTURE_LIST_H

#include "LispElement.h"
#include "ListMemento.h"
#include <vector>
#include <memory>
#include <iostream>
#include <stack>
#include <stdexcept>

class List : public LispElement {
private:
    std::vector<std::shared_ptr<LispElement>> elements;
    std::stack<std::shared_ptr<ListMemento>> history;
public:
    explicit List(const std::shared_ptr<LispElement>& rootElement = nullptr);

    void add(const std::shared_ptr<LispElement>& element);
    void insert(size_t index, const std::shared_ptr<LispElement>& element);
    void remove(size_t index);
    std::shared_ptr<LispElement> get(size_t index);

    void transactionStart();
    void transactionCommit();
    void transactionRollback();

protected:
    void recursiveLock(const std::shared_ptr<TransactionToken>& token) override;
    void recursiveUnlock() override;

    std::shared_ptr<Memento> createMemento() const override;
    void setMemento(const std::shared_ptr<Memento>& memento) override;

    void print() const override;
    void printUnsafe() const override;
};

#endif // LISPSTRUCTURE_LIST_H
