#include "Executor.h"
#include "../console/console.h"
#include "../TinyKernel/kernel.h"
#include "../Platform/fileio.h"
#include "../TinyKernel/process.h"
#include <string>
#include <cstdlib>

void Executor::execute(const Parser::Command& cmd) {
    if (cmd.program.empty()) return;
    
    if (cmd.program == "help") {
        console_write_line(console, "Available commands: help, clear, echo, version, exit, mkdir, touch, cat, run");
    } else if (cmd.program == "clear") {
        console_clear(console);
    } else if (cmd.program == "echo") {
        std::string out;
        for (size_t i = 0; i < cmd.arguments.size(); ++i) {
            out += cmd.arguments[i];
            if (i + 1 < cmd.arguments.size()) out += " ";
        }
        console_write_line(console, out.c_str());
    } else if (cmd.program == "version") {
        console_write_line(console, "TinyShell v1.0");
    } else if (cmd.program == "exit") {
        console_write_line(console, "Exit command not fully supported yet.");
    } else if (cmd.program == "mkdir") {
        if (cmd.arguments.empty()) {
            console_write_line(console, "mkdir: missing operand");
        } else {
            fileio_mkdir(cmd.arguments[0].c_str());
        }
    } else if (cmd.program == "touch") {
        if (cmd.arguments.empty()) {
            console_write_line(console, "touch: missing operand");
        } else {
            fileio_write(cmd.arguments[0].c_str(), "", 0);
        }
    } else if (cmd.program == "cat") {
        if (cmd.arguments.empty()) {
            console_write_line(console, "cat: missing operand");
        } else {
            size_t size = 0;
            unsigned char* data = fileio_read(cmd.arguments[0].c_str(), &size);
            if (data) {
                console_write_line(console, reinterpret_cast<char*>(data));
                free(data);
            } else {
                console_write_line(console, "cat: could not read file");
            }
        }
    } else if (cmd.program == "run") {
        if (cmd.arguments.empty()) {
            console_write_line(console, "run: missing executable path");
        } else {
            process_create(cmd.arguments[0].c_str());
        }
    } else {
        std::string err = "Unknown command: " + cmd.program;
        console_write_line(console, err.c_str());
    }
}
