#include "../console/console.h"
#include "shell.h"
#include "prompt.h"

#include <string>



extern ConsoleHandle* console;

void shell() {


    console_write(console, "Shell started!");

    // while (true){
    //     std::string current_dir = "~";
    //
    //     console_write(console, current_dir.c_str());
    //
    //     break;  // Temporary
    //
    // }
}