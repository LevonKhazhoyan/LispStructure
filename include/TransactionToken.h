#ifndef LISPSTRUCTURE_TRANSACTION_TOKEN_H
#define LISPSTRUCTURE_TRANSACTION_TOKEN_H

#include <thread>

class TransactionToken {
public:
    std::thread::id owner = std::this_thread::get_id();
};

#endif // LISPSTRUCTURE_TRANSACTION_TOKEN_H
