#ifndef LISPSTRUCTURE_LISP_ELEMENT_H
#define LISPSTRUCTURE_LISP_ELEMENT_H

#include <memory>
#include <mutex>
#include "Memento.h"
#include "TransactionToken.h"

class LispElement {
protected:
    std::shared_ptr<LispElement> root;
public:
    std::shared_ptr<TransactionToken> activeTransactionToken;
    mutable std::recursive_mutex mtx;

    virtual ~LispElement() = default;
    virtual void print() const = 0;
    virtual void printUnsafe() const = 0;
    virtual std::string getValue() const = 0;
    virtual std::shared_ptr<LispElement> getByIndex(size_t index) const = 0;
    virtual void recursiveLock(const std::shared_ptr<TransactionToken>& token);
    virtual void recursiveUnlock();
    void setRoot(const std::shared_ptr<LispElement>& rootElement);
    virtual std::shared_ptr<LispElement> getRoot() const;
    virtual std::shared_ptr<Memento> createMemento() const = 0;
    virtual void setMemento(const std::shared_ptr<Memento>& memento) = 0;
};

#endif // LISPSTRUCTURE_LISP_ELEMENT_H
