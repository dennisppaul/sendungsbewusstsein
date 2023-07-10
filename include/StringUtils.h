#pragma once

#include <string>
#include <sstream>

std::vector<std::string> split(const std::string& input, const std::string& delimiter) {
    std::vector<std::string> tokens;
    std::size_t start = 0;
    std::size_t end = 0;

    while ((end = input.find(delimiter, start)) != std::string::npos) {
        tokens.push_back(input.substr(start, end - start));
        start = end + delimiter.length();
    }

    tokens.push_back(input.substr(start));
    return tokens;
}

std::vector<std::string> split(std::string &input, char delimiter) {
    std::vector<std::string> tokens;

    std::istringstream iss(input);
    std::string token;

    while (std::getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}