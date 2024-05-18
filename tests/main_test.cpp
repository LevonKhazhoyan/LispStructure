#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "Atom.h"
#include "List.h"

using namespace std;

TEST_CASE("Simple Transaction commit test", "[transactionStart]") {
    auto root = std::make_shared<List>();
    root->transactionStart();
    root->add(std::make_shared<Atom>("hello"));
    root->add(std::make_shared<Atom>("world"));
    root->transactionCommit();
    REQUIRE("String" == root->get(0));
}
