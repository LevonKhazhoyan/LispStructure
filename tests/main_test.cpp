#include <catch2/catch_all.hpp>

#include "Atom.h"
#include "List.h"

using namespace std;

TEST_CASE("Simple Transaction commit test", "[transactionStart]") {
    //def
    auto root = std::make_shared<List>();
    root->transactionStart();
    std::string expectedValue = "Hello";
    root->add(std::make_shared<Atom>(expectedValue));
    root->transactionCommit();
    //res
    auto returnedAtom = std::dynamic_pointer_cast<Atom>(root->getByIndex(0));
    //assert
    REQUIRE(returnedAtom != nullptr);
    REQUIRE(returnedAtom->getValue() == expectedValue);
}

TEST_CASE("Rollback test", "[transactionRollback]") {
    //def
    auto root = std::make_shared<List>();
    std::string expectedToBeInValue = "Hello";
    root->add(std::make_shared<Atom>(expectedToBeInValue));
    root->transactionStart();
    std::string notExpectedToBeInValue = "World";
    root->add(std::make_shared<Atom>(notExpectedToBeInValue));
    root->transactionRollback();

    //res
    auto returnedAtomBeforeRollback = std::dynamic_pointer_cast<Atom>(root->getByIndex(0));
    auto returnedAtomAfterRollback = std::dynamic_pointer_cast<Atom>(root->getByIndex(1));

    //assert
    REQUIRE(returnedAtomBeforeRollback != nullptr);
    REQUIRE(returnedAtomBeforeRollback->getValue() == expectedToBeInValue);
    REQUIRE(returnedAtomAfterRollback == nullptr);
}

TEST_CASE("Get root test", "[getRoot]") {
    //def
    auto root = std::make_shared<List>();
    auto sublist = std::make_shared<List>(root->getRoot());
    root->add(sublist);
    sublist->add(std::make_shared<Atom>("nested"));
    sublist->add(std::make_shared<Atom>("list"));

    //res
    auto returnedRoot = sublist->getRoot();

    //assert
    REQUIRE(returnedRoot == root);
}

TEST_CASE("Inner transaction works", "[transactionInner]") {
    //def
    auto root = std::make_shared<List>();
    auto sublist = std::make_shared<List>(root->getRoot());
    root->add(sublist);
    sublist->add(std::make_shared<Atom>("nested"));

    auto sublist2 = std::make_shared<List>(root);
    sublist->add(sublist2);
    sublist2->add(std::make_shared<Atom>("nestednested"));
    std::thread t1([&]{
        sublist->transactionStart();
        sublist->add(std::make_shared<Atom>("threadOneBeforeSleep"));
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        sublist->add(std::make_shared<Atom>("threadOneAfterSleep"));
        sublist->transactionRollback();
    });

    std::thread t2([&]{
        sublist2->add(std::make_shared<Atom>("Before transaction"));
        sublist2->transactionStart();
        auto sublist3 = std::make_shared<List>(root->getRoot());
        sublist3->add(std::make_shared<Atom>("threadTwo"));
        sublist2->add(sublist3);
        sublist2->transactionCommit();
    });

    //res
    t1.join();
    t2.join();

    //just assert that nothing fails
    REQUIRE(root->getByIndex(0)->getByIndex(1)->getByIndex(2)->getByIndex(0)->getValue() == "threadTwo");
}
