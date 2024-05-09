#include <iostream>
#include <utility>
#include <stack>
#include <thread>

class Memento {
public:
    virtual ~Memento() = default;
    virtual void printHistory() const = 0;
};

class TransactionToken {
public:
    std::thread::id owner = std::this_thread::get_id();
};

class LispElement {
protected:
    std::shared_ptr<LispElement> root;
public:
    std::shared_ptr<TransactionToken> activeTransactionToken;
    mutable std::recursive_mutex mtx;
    virtual ~LispElement() = default;
    virtual void print() const = 0;
    virtual void printUnsafe() const = 0;
    virtual void printAll() const = 0;
    virtual void printAllUnsafe() const = 0;
    virtual void recursiveLock(const std::shared_ptr<TransactionToken>& token) {
        mtx.lock();
        activeTransactionToken = token;
    }
    virtual void recursiveUnlock() {
        activeTransactionToken.reset();
        mtx.unlock();
    }
    void setRoot(const std::shared_ptr<LispElement>& rootElement) {
        root = rootElement;
    }
    std::shared_ptr<LispElement> getRoot() const {
        return root;
    }
    virtual std::shared_ptr<Memento> createMemento() const = 0;
    virtual void setMemento(const std::shared_ptr<Memento>& memento) = 0;
};

class AtomMemento : public Memento {
    std::string state;
public:
    explicit AtomMemento(std::string state) : state(std::move(state)) {}
    void restore(std::string& stateToRestore) {
        stateToRestore = state;
    }
    void printHistory() const override {
        std::cout << state;
    }
};

class Atom : public LispElement {
private:
    std::string value;
protected:
    Atom(std::string value, const std::shared_ptr<LispElement>& rootElement) : value(std::move(value)) {
        setRoot(rootElement);
    }
public:
    explicit Atom(std::string value) : value(std::move(value)) {}
    void print() const override {
        mtx.lock();
        printUnsafe();
        mtx.unlock();
    }
    void printUnsafe() const override {
        std::cout << value;
    }
    void printAllUnsafe() const override {
        std::cout << value;
    }
    void printAll() const override {
        std::cout << value;
    }
    std::shared_ptr<Memento> createMemento() const override {
        return std::make_shared<AtomMemento>(value);
    }
    void setMemento(const std::shared_ptr<Memento>& memento) override {
        std::static_pointer_cast<AtomMemento>(memento)->restore(value);
    }
};

class ListMemento : public Memento {
    std::vector<std::shared_ptr<LispElement>> state;
public:
    explicit ListMemento(const std::vector<std::shared_ptr<LispElement>>& state) : state(state) {}
    void restore(std::vector<std::shared_ptr<LispElement>>& stateToRestore) {
        stateToRestore = state;
    }
    void printHistory() const override {
        std::cout << "(";
        for (const auto& elem : state) {
            elem->printUnsafe();
            std::cout << " ";
        }
        std::cout << ")";
    }
};

class List : public LispElement {
private:
    std::vector<std::shared_ptr<LispElement>> elements;
    std::stack<std::shared_ptr<ListMemento>> history;
public:
    explicit List(const std::shared_ptr<LispElement>& rootElement = nullptr) {
        setRoot(rootElement ? rootElement : std::shared_ptr<LispElement>(this, [](auto _){}));
    }

    void add(const std::shared_ptr<LispElement>& element) {
        mtx.lock();
        element->setRoot(getRoot());
        elements.push_back(element);
        mtx.unlock();
    }

    void insert(size_t index, const std::shared_ptr<LispElement>& element) {
        element->setRoot(getRoot());
        if (index <= elements.size()) {
            elements.insert(elements.begin() + index, element);
        }
    }

    void remove(size_t index) {
        if (index < elements.size()) {
            elements.erase(elements.begin() + index);
        }
    }

    std::shared_ptr<LispElement> get(size_t index) {
        if (index < elements.size()) {
            return elements[index];
        }
        return nullptr;
    }

    void transactionStart() {
        if (activeTransactionToken) {
            throw std::runtime_error("Transaction already active on this or a nested node");
        }
        auto newToken = std::make_shared<TransactionToken>();
        recursiveLock(newToken);
        history.push(std::make_shared<ListMemento>(elements));
    }

    void recursiveLock(const std::shared_ptr<TransactionToken>& token) override {
        if (activeTransactionToken) {
            throw std::runtime_error("An existing transaction is already using this node");
        }
        mtx.lock();
        activeTransactionToken = token;
        for (auto& elem : elements) {
            elem->recursiveLock(token);
        }
    }

    void recursiveUnlock() override {
        for (auto& elem : elements) {
            elem->recursiveUnlock();
        }
        mtx.unlock();
        activeTransactionToken.reset();
    }

    void transactionCommit() {
        if (!history.empty()) {
            recursiveUnlock();
            history.pop();
        } else {
            throw std::runtime_error("No transaction to commit");
        }
    }

    void transactionRollback() {
        if (!history.empty()) {
            std::shared_ptr<ListMemento> memento = history.top();
            memento->restore(elements);
            history.pop();
            recursiveUnlock();
        } else {
            throw std::runtime_error("No transaction to rollback");
        }
    }

    std::shared_ptr<Memento> createMemento() const override {
        return std::make_shared<ListMemento>(elements);
    }

    void setMemento(const std::shared_ptr<Memento>& memento) override {
        std::static_pointer_cast<ListMemento>(memento)->restore(elements);
    }

    void print() const override {
        mtx.lock();
        printUnsafe();
        mtx.unlock();
    }

    void printUnsafe() const override {
        std::cout << "(";
        for (const auto& elem : elements) {
            elem->printUnsafe();
            std::cout << " ";
        }
        std::cout << ")";
    }

    void printAll() const override {
        mtx.lock();
        std::cout << "[history: ";
        printAllUnsafe();
        std::cout << "]";
        mtx.unlock();
    }

    void printAllUnsafe() const override {
        for (const auto& elem : elements) {
            elem->printAllUnsafe();
            history.top()->printHistory();
        }
        printUnsafe();
    }
};


int main() {

    auto root = std::make_shared<List>();
    root->add(std::make_shared<Atom>("hello"));
    root->add(std::make_shared<Atom>("world"));

    auto sublist = std::make_shared<List>(root->getRoot());
    root->add(sublist);
    sublist->add(std::make_shared<Atom>("nested"));
    sublist->add(std::make_shared<Atom>("list"));

    auto sublist2 = std::make_shared<List>(root->getRoot());
    sublist->add(sublist2);
    sublist2->add(std::make_shared<Atom>("nested2"));
    sublist2->add(std::make_shared<Atom>("list2"));
    std::thread t1([&]{
        sublist->transactionStart();
        sublist->add(std::make_shared<Atom>("thread 1"));
        std::this_thread::sleep_for(std::chrono::milliseconds(900));
        sublist->add(std::make_shared<Atom>("thread 1"));
        std::cout << "\nPrint after 1 thread: ";
        sublist->getRoot()->print();
        std::cout << std::endl;
        sublist->transactionRollback();
    });

    std::thread t2([&]{
        sublist2->add(std::make_shared<Atom>("from"));
        sublist2->transactionStart();
        auto sublist3 = std::make_shared<List>(root->getRoot());
        sublist3->add(std::make_shared<Atom>("thread2list3"));
        sublist2->add(sublist3);
        sublist3->remove(0);
        sublist3->insert(0, std::make_shared<Atom>("thread2list4"));
        sublist2->add(std::make_shared<Atom>("thread 2"));
        std::cout << "\nPrint after 2 thread: ";
        sublist->getRoot()->print();
        std::cout << std::endl;
        sublist2->transactionCommit();
    });

    t1.join();
    t2.join();
    std::cout << "\nPrint after all threads: ";
    sublist->getRoot()->print();
    std::cout << std::endl;
    return 0;
}