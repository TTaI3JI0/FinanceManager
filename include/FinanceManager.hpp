#pragma once

#include <vector>
#include "Transaction.hpp"

class FinanceManager {
public:
    FinanceManager() = default;

    void addTransaction(const Transaction &transaction);
    const std::vector<Transaction> &getTransactions() const;
    void printSummary() const;

private:
    std::vector<Transaction> transactions_;
};

