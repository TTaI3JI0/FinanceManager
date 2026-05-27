#pragma once

#include <string>

struct Category {
    std::string name;
    bool isIncome{};

    Category() = default;

    Category(const std::string &name, bool isIncome)
        : name(name), isIncome(isIncome) {}
};

