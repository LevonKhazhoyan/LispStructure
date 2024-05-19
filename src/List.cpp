#include "List.h"

List::List(const std::shared_ptr<LispElement>& rootElement) {
    setRoot(rootElement ? rootElement : std::shared_ptr<LispElement>(this, [](auto _){}));
}

void List::add(const std::shared_ptr<LispElement>& element) {
    mtx.lock();
    element->setRoot(getRoot());
    elements.push_back(element);
    mtx.unlock();
}

void List::insert(size_t index, const std::shared_ptr<LispElement>& element) {
    element->setRoot(getRoot());
    if (index <= elements.size()) {
        elements.insert(elements.begin() + index, element);
    }
}

void List::remove(size_t index) {
    if (index < elements.size()) {
        elements.erase(elements.begin() + index);
    }
}

std::shared_ptr<LispElement> List::getByIndex(size_t index) const {
    if (index < elements.size()) {
        return elements[index];
    }
    return nullptr;
}

std::string List::getValue() const {
    throw std::runtime_error("Not allowed operation");
}

void List::transactionStart() {
    if (activeTransactionToken && activeTransactionToken->owner != std::this_thread::get_id()) {
        throw std::runtime_error("Transaction already active on this or a nested node");
    }
    auto newToken = std::make_shared<TransactionToken>();
    recursiveLock(newToken);
    history.push(std::make_shared<ListMemento>(elements));
}

void List::recursiveLock(const std::shared_ptr<TransactionToken>& token) {
    mtx.lock();
    if (activeTransactionToken) {
        throw std::runtime_error("An existing transaction is already using this node");
    }
    activeTransactionToken = token;
    for (auto& elem : elements) {
        elem->recursiveLock(token);
    }
}

void List::recursiveUnlock() {
    for (auto& elem : elements) {
        elem->recursiveUnlock();
    }
    mtx.unlock();
    activeTransactionToken.reset();
}

void List::transactionCommit() {
    if (!history.empty()) {
        recursiveUnlock();
        history.pop();
    } else {
        throw std::runtime_error("No transaction to commit");
    }
}

void List::transactionRollback() {
    if (!history.empty()) {
        std::shared_ptr<ListMemento> memento = history.top();
        memento->restore(elements);
        history.pop();
        recursiveUnlock();
    } else {
        throw std::runtime_error("No transaction to rollback");
    }
}

std::shared_ptr<Memento> List::createMemento() const {
    return std::make_shared<ListMemento>(elements);
}

void List::setMemento(const std::shared_ptr<Memento>& memento) {
    std::static_pointer_cast<ListMemento>(memento)->restore(elements);
}

void List::print() const {
    mtx.lock();
    printUnsafe();
    mtx.unlock();
}

void List::printUnsafe() const {
    std::cout << "(";
    for (const auto& elem : elements) {
        elem->printUnsafe();
        std::cout << " ";
    }
    std::cout << ")";
}
