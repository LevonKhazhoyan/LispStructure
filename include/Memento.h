#ifndef LISPSTRUCTURE_MEMENTO_H
#define LISPSTRUCTURE_MEMENTO_H

class Memento {
public:
    virtual ~Memento() = default;
    virtual void printHistory() const = 0;
};

#endif // LISPSTRUCTURE_MEMENTO_H