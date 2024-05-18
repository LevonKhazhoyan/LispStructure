#include "Atom.h"
#include "AtomMemento.h"
#include <iostream>
#include <memory>

Atom::Atom(std::string value, const std::shared_ptr<LispElement>& rootElement) : value(std::move(value)) {
    setRoot(rootElement);
}

Atom::Atom(std::string value) : value(std::move(value)) {}

void Atom::print() const {
    mtx.lock();
    printUnsafe();
    mtx.unlock();
}

void Atom::printUnsafe() const {
    std::cout << value;
}

std::string Atom::getValue() const {
    return value;
}

std::shared_ptr<Memento> Atom::createMemento() const {
    return std::make_shared<AtomMemento>(value);
}

void Atom::setMemento(const std::shared_ptr<Memento>& memento) {
    std::static_pointer_cast<AtomMemento>(memento)->restore(value);
}


std::shared_ptr<LispElement> Atom::getByIndex(size_t index) const {
    throw std::runtime_error("Not allowed operation");
}

