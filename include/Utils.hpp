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

bool isValidDate(const std::string &date);

// Reads a date until YYYY-MM-DD is valid.
std::string readValidDate(const std::string &prompt);

// Reads a date or empty line (empty means "keep current value" in edit flows).
std::string readOptionalValidDate(const std::string &prompt);

void printError(const std::string &message);
void printSuccess(const std::string &message);
void printInfo(const std::string &message);

void printHorizontalBarChart(const std::map<std::string, double> &data, int width = 40);
