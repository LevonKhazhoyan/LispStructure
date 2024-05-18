#include "Atom.h"
#include "List.h"


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