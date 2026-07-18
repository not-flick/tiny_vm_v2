#include "prompt.h"

#include <ctype.h>

int prompt_tokenize(char* input, PromptTokens* output)
{
    if (!input || !output)
        return 0;

    output->count = 0;

    char* p = input;

    while (*p)
    {
        /* Skip whitespace */
        while (*p && isspace((unsigned char)*p))
        {
            *p = '\0';
            p++;
        }

        if (!*p)
            break;

        if (output->count >= PROMPT_MAX_TOKENS)
            break;

        output->tokens[output->count++] = p;

        /* Find end of token */
        while (*p && !isspace((unsigned char)*p))
            p++;
    }

    return output->count;
}