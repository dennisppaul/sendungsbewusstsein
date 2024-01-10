#pragma once

#include <string>
#include <sstream>
#include <vector>

bool equals(const char *str1, const char *str2);

bool equals(const std::string &str1, const char *str2);

bool equals_ignore_case(const std::string &str1, const std::string &str2);

bool starts_with_ignore_case(const std::string &str, const std::string &starts_with);

std::vector<std::string> split(const std::string &input, const std::string &delimiter);

std::vector<std::string> split(std::string &input, char delimiter);
