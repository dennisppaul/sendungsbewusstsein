#pragma once

#include <string>
#include <sstream>

bool equals_ignore_case(const std::string &str1, const std::string &str2) {
    if (str1.length() != str2.length())
        return false;

    for (size_t i = 0; i < str1.length(); ++i) {
        if (std::tolower(str1[i]) != std::tolower(str2[i]))
            return false;
    }

    return true;
}

bool starts_with_ignore_case(const std::string &str, const std::string &starts_with) {
    if (str.length() < starts_with.length())
        return false;

    return std::equal(starts_with.begin(), starts_with.end(), str.begin(),
                      [](char a, char b) {
                          return std::tolower(a) == std::tolower(b);
                      });
}

std::vector<std::string> split(const std::string &input, const std::string &delimiter) {
    std::vector<std::string> tokens;

    std::regex                 regex(R"(("[^"]*")|('[^']*')|()" + delimiter + ")");
    std::sregex_token_iterator iter(input.begin(), input.end(), regex, {-1, 0});
    std::sregex_token_iterator end;

    for (; iter != end; ++iter) {
        if (!iter->str().empty() && iter->str() != delimiter) {
            tokens.push_back(*iter);
        }
    }

    return tokens;
}

//std::vector<std::string> _split(const std::string &input, const std::string &delimiter) {
//    std::vector<std::string> tokens;
//    std::size_t              start = 0;
//    std::size_t              end   = 0;
//
//    while ((end = input.find(delimiter, start)) != std::string::npos) {
//        tokens.push_back(input.substr(start, end - start));
//        start = end + delimiter.length();
//    }
//
//    tokens.push_back(input.substr(start));
//    return tokens;
//}

std::vector<std::string> split(std::string &input, char delimiter) {
    std::vector<std::string> tokens;

    std::istringstream iss(input);
    std::string        token;

    while (std::getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}