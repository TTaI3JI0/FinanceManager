#pragma once

#include <map>
#include <string>

namespace Ansi {
constexpr const char *Reset = "\033[0m";
constexpr const char *Bold = "\033[1m";
constexpr const char *Red = "\033[31m";
constexpr const char *Green = "\033[32m";
constexpr const char *Yellow = "\033[33m";
constexpr const char *Blue = "\033[34m";
constexpr const char *Magenta = "\033[35m";
constexpr const char *Cyan = "\033[36m";
} // namespace Ansi

// Enables ANSI colors on Windows terminals (no-op on other platforms).
void enableAnsiSupport();

// Validates user input format DD.MM.YYYY
bool isValidDate(const std::string &date);

// Converts DD.MM.YYYY -> YYYY-MM-DD (for database storage and SQL queries).
std::string dateInputToIso(const std::string &date);

// Converts YYYY-MM-DD -> DD.MM.YYYY (for display).
std::string dateIsoToDisplay(const std::string &isoDate);

// Reads a date until DD.MM.YYYY is valid; returns YYYY-MM-DD.
std::string readValidDate(const std::string &prompt);

// Reads a date or empty line; returns "" or YYYY-MM-DD.
std::string readOptionalValidDate(const std::string &prompt);

void printError(const std::string &message);
void printSuccess(const std::string &message);
void printInfo(const std::string &message);

void printHorizontalBarChart(const std::map<std::string, double> &data, int width = 40);
