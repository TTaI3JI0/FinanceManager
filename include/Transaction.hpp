#pragma once

#include <string>

struct Transaction {
    int id;
    std::string date;        // stored as YYYY-MM-DD in database
    double amount;
    std::string category;
    std::string description;

    Transaction() = default;

    Transaction(int id,
                const std::string &date,
                double amount,
                const std::string &category,
                const std::string &description)
        : id(id),
          date(date),
          amount(amount),
          category(category),
          description(description) {}
};

