#include "page.h"

#include <stdlib.h>

#define PAGE_SIZE 4096

Page *getpages(Memory *memory)
{
    if (!memory)
        return NULL;

    size_t count = memory_size(memory) / PAGE_SIZE;

    Page *pages = (Page *)malloc(sizeof(Page) * count);
    if (!pages)
        return NULL;

    for (size_t i = 0; i < count; i++)
    {
        pages[i].offset = i * PAGE_SIZE;
    }

    return pages;
}

void freepages(Page *pages)
{
    free(pages);
}