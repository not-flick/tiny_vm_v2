#pragma once
#include <string>
#include <vector>

class Parser {
public:
    struct Command {
        std::string program;
        std::vector<std::string> arguments;
    };
    Command parse(const std::string& input);
};
