#pragma once

#include <string>
#include <vector>
#include "Category.hpp"
#include "Transaction.hpp"

class FinanceManager {
public:
    FinanceManager();

    void addTransaction(const Transaction &transaction);
    const std::vector<Transaction> &getTransactions() const;
    void printSummary() const;

    void addCategory(const Category &category);
    void printAllCategories() const;
    bool categoryExists(const std::string &name) const;
    const std::vector<Category> &getCategories() const;

private:
    std::vector<Category> categories_;
    std::vector<Transaction> transactions_;
};

