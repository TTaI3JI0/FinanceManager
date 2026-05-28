#pragma once

#include <map>
#include <string>
#include <vector>
#include "DatabaseManager.hpp"
#include "Transaction.hpp"

class FinanceManager {
public:
    explicit FinanceManager(DatabaseManager &db);

    bool addTransaction(const std::string &date,
                        double amount,
                        const std::string &categoryName,
                        const std::string &description);
    bool removeTransaction(int id);
    std::vector<Transaction> getAllTransactions();
    void printAllTransactions();

    bool addCategory(const std::string &name, bool isIncome);
    void printAllCategories();
    bool categoryExists(const std::string &name) const;

    double getTotalBalance() const;
    std::map<std::string, double> getExpensesByCategory() const;
    double getBalanceForPeriod(const std::string &startDate, const std::string &endDate) const;

private:
    DatabaseManager &db_;
};
