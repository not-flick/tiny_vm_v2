#include "../console/console.h"
#include "shell.h"
#include "prompt.h"
#include "../TinyVM/tinyvm.h"

#include <string>



extern ConsoleHandle* console;

std::string prompt;

void shell() 
{


    // console_write(console, "Shell started!");

        std::string current_dir = username;

        console_write(console, current_dir.c_str());
    
    
}