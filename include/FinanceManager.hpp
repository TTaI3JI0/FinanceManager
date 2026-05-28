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
    bool editTransaction(int id,
                         const std::string &newDate,
                         double newAmount,
                         const std::string &newCategoryName,
                         const std::string &newDescription,
                         bool updateAmount = true);
    std::vector<Transaction> getAllTransactions();
    void printAllTransactions();

    bool addCategory(const std::string &name, bool isIncome);
    void printAllCategories();
    bool categoryExists(const std::string &name) const;

    double getTotalBalance() const;
    std::map<std::string, double> getExpensesByCategory() const;
    double getBalanceForPeriod(const std::string &startDate, const std::string &endDate) const;
    std::map<std::string, double> getMonthlyExpenses() const;

    std::vector<Transaction> findTransactionsByDescription(const std::string &substr) const;
    std::vector<Transaction> findTransactionsByCategory(const std::string &categoryName) const;
    std::vector<Transaction> findTransactionsByDateRange(const std::string &fromDate,
                                                         const std::string &toDate) const;
    std::vector<Transaction> findTransactionsByAmountRange(double minAmount, double maxAmount) const;

private:
    std::vector<Transaction> queryTransactions(const std::string &sql,
                                               const std::vector<SqlBind> &binds) const;

    DatabaseManager &db_;
};
