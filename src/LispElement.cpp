#include "LispElement.h"

void LispElement::recursiveLock(const std::shared_ptr<TransactionToken>& token) {
    mtx.lock();
    activeTransactionToken = token;
}

void LispElement::recursiveUnlock() {
    activeTransactionToken.reset();
    mtx.unlock();
}

void LispElement::setRoot(const std::shared_ptr<LispElement>& rootElement) {
    root = rootElement;
}

std::shared_ptr<LispElement> LispElement::getRoot() const {
    return root;
}