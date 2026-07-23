#pragma once
#include "Parser.h"

class Executor {
public:
    void execute(const Parser::Command& cmd);
};
