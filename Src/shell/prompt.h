#pragma once

#ifndef PROMPT_H
#define PROMPT_H

#ifdef __cplusplus
extern "C" {
#endif

#define PROMPT_MAX_TOKENS 32

    typedef struct
    {
        char* tokens[PROMPT_MAX_TOKENS];
        int count;
    } PromptTokens;

    /*
     * Tokenizes the input string.
     *
     * The input string is modified in-place:
     * "ls -la /home"
     * becomes
     * "ls\0-la\0/home\0"
     *
     * Returns the number of tokens.
     */
    int prompt_tokenize(char* input, PromptTokens* output);

#ifdef __cplusplus
}
#endif

#endif