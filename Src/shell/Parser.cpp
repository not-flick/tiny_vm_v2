#include "Parser.h"
#include "prompt.h"
#include <cstring>

Parser::Command Parser::parse(const std::string& input) {
    Command cmd;
    if (input.empty()) return cmd;
    
    char* buffer = new char[input.length() + 1];
    std::strcpy(buffer, input.c_str());
    
    PromptTokens tokens;
    prompt_tokenize(buffer, &tokens);
    
    if (tokens.count > 0) {
        cmd.program = tokens.tokens[0];
        for (int i = 1; i < tokens.count; ++i) {
            cmd.arguments.push_back(tokens.tokens[i]);
        }
    }
    delete[] buffer;
    return cmd;
}
