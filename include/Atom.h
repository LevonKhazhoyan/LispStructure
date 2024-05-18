#ifndef LISPSTRUCTUREATOM_H
#define LISPSTRUCTUREATOM_H

#include "LispElement.h"
#include <string>

class Atom : public LispElement {
private:
    std::string value;
protected:
    Atom(std::string value, const std::shared_ptr<LispElement>& rootElement);
public:
    explicit Atom(std::string value);
    void print() const override;
    void printUnsafe() const override;
    std::shared_ptr<Memento> createMemento() const override;
    void setMemento(const std::shared_ptr<Memento>& memento) override;
};

#endif // LISPSTRUCTUREATOM_H
