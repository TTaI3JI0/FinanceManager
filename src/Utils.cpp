#include "Utils.hpp"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

void enableAnsiSupport() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    DWORD mode = 0;
    if (!GetConsoleMode(hOut, &mode)) return;

    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, mode);
#endif
}

namespace {

bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int daysInMonth(int year, int month) {
    static const int days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month < 1 || month > 12) return 0;
    if (month == 2 && isLeapYear(year)) return 29;
    return days[month];
}

} // namespace

bool isValidDate(const std::string &date) {
    if (date.size() != 10) return false;
    if (date[4] != '-' || date[7] != '-') return false;

    for (int i = 0; i < 10; ++i) {
        if (i == 4 || i == 7) continue;
        if (!std::isdigit(static_cast<unsigned char>(date[i]))) return false;
    }

    try {
        const int year = std::stoi(date.substr(0, 4));
        const int month = std::stoi(date.substr(5, 2));
        const int day = std::stoi(date.substr(8, 2));

        if (year < 1 || year > 9999) return false;
        if (month < 1 || month > 12) return false;
        if (day < 1 || day > daysInMonth(year, month)) return false;
        return true;
    } catch (...) {
        return false;
    }
}

std::string readValidDate(const std::string &prompt) {
    while (true) {
        std::string date;
        std::cout << prompt;
        std::cin >> date;

        if (isValidDate(date)) {
            return date;
        }

        printError("Invalid date. Use format YYYY-MM-DD.");
    }
}

std::string readOptionalValidDate(const std::string &prompt) {
    while (true) {
        std::string date;
        std::cout << prompt;
        std::getline(std::cin >> std::ws, date);

        if (date.empty()) {
            return "";
        }

        if (isValidDate(date)) {
            return date;
        }

        printError("Invalid date. Use format YYYY-MM-DD or leave empty.");
    }
}

void printError(const std::string &message) {
    std::cout << Ansi::Red << message << Ansi::Reset << "\n";
}

void printSuccess(const std::string &message) {
    std::cout << Ansi::Green << message << Ansi::Reset << "\n";
}

void printInfo(const std::string &message) {
    std::cout << Ansi::Cyan << message << Ansi::Reset << "\n";
}

void printHorizontalBarChart(const std::map<std::string, double> &data, int width) {
    if (data.empty()) {
        printInfo("No data to display.");
        return;
    }

    if (width < 5) width = 5;

    double maxValue = 0.0;
    for (const auto &kv : data) {
        maxValue = std::max(maxValue, kv.second);
    }
    if (maxValue <= 0.0) maxValue = 1.0;

    std::cout << std::fixed << std::setprecision(2);
    for (const auto &kv : data) {
        int barLength = static_cast<int>((kv.second / maxValue) * width);
        if (kv.second > 0.0 && barLength == 0) barLength = 1;

        std::cout << Ansi::Bold << Ansi::Blue << kv.first << Ansi::Reset << " ";
        std::cout << Ansi::Yellow;
        for (int i = 0; i < barLength; ++i) {
            std::cout << '#';
        }
        std::cout << Ansi::Reset << " " << kv.second << "\n";
    }
}
