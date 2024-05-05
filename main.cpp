#include <iostream>
#include <utility>
#include <stack>
#include <thread>

class Memento {
public:
    virtual ~Memento() {}
};

class LispElement {
protected:
    std::shared_ptr<LispElement> root;
public:
    mutable std::recursive_mutex mtx;
    virtual ~LispElement() = default;
    virtual void print() const = 0;
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
    AtomMemento(std::string state) : state(std::move(state)) {}
    void restore(std::string& stateToRestore) {  // Rewritten to specifically work with string
        stateToRestore = state;
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
        std::lock_guard<std::recursive_mutex> lock(mtx);  // Lock when accessing value
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
    ListMemento(const std::vector<std::shared_ptr<LispElement>>& state) : state(state) {}
    void restore(std::vector<std::shared_ptr<LispElement>>& stateToRestore) {  // Rewritten to work with vector
        stateToRestore = state;
    }
};

class List : public LispElement {
private:
    std::vector<std::shared_ptr<LispElement>> elements;
    std::stack<std::shared_ptr<Memento>> history;
public:
    explicit List(const std::shared_ptr<LispElement>& rootElement = nullptr) {
        setRoot(rootElement ? rootElement : std::shared_ptr<LispElement>(this, [](auto _){}));
    }

    void add(const std::shared_ptr<LispElement>& element) {
        std::lock_guard<std::recursive_mutex> lock(mtx);  // Lock when modifying list
        element->setRoot(getRoot());
        elements.push_back(element);
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
        mtx.lock();  // Explicitly lock the current node
        for (auto& elem : elements) {
            elem->mtx.lock();  // Recursively lock all child elements
        }
        history.push(createMemento());
    }

    void transactionCommit() {
        for (auto& elem : elements) {
            elem->mtx.unlock();  // Recursively unlock all child elements
        }
        mtx.unlock();  // Unlock the current node
        if (!history.empty()) {
            history.pop();
        } else {
            throw std::runtime_error("No transaction to commit");
        }
    }

    void transactionRollback() {
        for (auto& elem : elements) {
            elem->mtx.unlock();
        }
        mtx.unlock();
        if (!history.empty()) {
            setMemento(history.top());
            history.pop();
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
        std::lock_guard<std::recursive_mutex> lock(mtx);
        std::cout << "(";
        for (const auto& elem : elements) {
            elem->print();
            std::cout << " ";
        }
        std::cout << ")";
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

    std::thread t1([&]{
        sublist->transactionStart();
        sublist->add(std::make_shared<Atom>("thread 1"));
        std::this_thread::sleep_for(std::chrono::milliseconds(900));
        sublist->add(std::make_shared<Atom>("thread 1"));
        std::cout << "\nRoot accessed from sublist: ";
        sublist->getRoot()->print();  // Should print the whole structure
        std::cout << std::endl;
        sublist->transactionCommit();
    });

    std::thread t2([&]{
        sublist->transactionStart();
        sublist->add(std::make_shared<Atom>("from"));
        sublist->add(std::make_shared<Atom>("thread 2"));
        sublist->transactionCommit();
    });

    t1.join();
    t2.join();

    std::cout << "\nRoot accessed from sublist: ";
    sublist->getRoot()->print();  // Should print the whole structure
    std::cout << std::endl;
    return 0;
}