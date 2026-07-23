#include "Executor.h"
#include "../console/console.h"
#include "../TinyKernel/kernel.h"
#include "../Platform/fileio.h"
#include "../TinyKernel/process.h"
#include "../TinyKernel/ramauto.h"
#include <string>
#include <cstdlib>

void Executor::execute(const Parser::Command& cmd) {
    if (cmd.program.empty()) return;
    
    if (cmd.program == "help") {
        console_write_line(console, "Available commands: help, clear, echo, version, exit, mkdir, touch, cat, run, setram, ram");
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
    } else if (cmd.program == "setram") {
        if (cmd.arguments.empty()) {
            console_write_line(console, "setram: missing argument");
        } else {
            std::string arg = cmd.arguments[0];
            if (arg == "auto") {
                fileio_write("tinyvm.conf", "auto", 4);
                console_write_line(console, "Automatic RAM allocation restored.");
                console_write_line(console, "Restart TinyVM for the new memory size to take effect.");
            } else {
                char unit = arg.back();
                uint64_t multiplier = 1;
                if (unit == 'k' || unit == 'K') multiplier = 1024ULL;
                else if (unit == 'm' || unit == 'M') multiplier = 1024ULL * 1024ULL;
                else if (unit == 'g' || unit == 'G') multiplier = 1024ULL * 1024ULL * 1024ULL;
                
                std::string numPart = arg;
                if (multiplier != 1) numPart.pop_back();
                
                try {
                    uint64_t val = std::stoull(numPart);
                    uint64_t bytes = val * multiplier;
                    uint64_t host_ram = ramauto_host_ram();
                    if (bytes == 0) {
                        console_write_line(console, "setram: RAM size must be greater than zero");
                    } else if (bytes < TVM_MIN_RAM) {
                        console_write_line(console, ("setram: minimum supported RAM is " + std::string(ramauto_to_string(TVM_MIN_RAM))).c_str());
                    } else if (bytes > host_ram) {
                        console_write_line(console, ("setram: cannot allocate more than host RAM (" + std::string(ramauto_to_string(host_ram)) + ")").c_str());
                    } else {
                        std::string bytesStr = std::to_string(bytes);
                        fileio_write("tinyvm.conf", bytesStr.c_str(), bytesStr.length());
                        console_write_line(console, ("Virtual RAM configured to " + std::string(ramauto_to_string(bytes)) + ".").c_str());
                        console_write_line(console, "Restart TinyVM for the new memory size to take effect.");
                    }
                } catch (...) {
                    console_write_line(console, "setram: malformed value");
                }
            }
        }
    } else if (cmd.program == "ram") {
        uint64_t host_ram = ramauto_host_ram();
        
        uint64_t configured_ram = TVM_RAM_AUTO;
        bool is_auto = true;
        size_t conf_size = 0;
        unsigned char* conf_data = fileio_read("tinyvm.conf", &conf_size);
        if (conf_data) {
            if (conf_size > 0) {
                std::string conf_str((char*)conf_data, conf_size);
                if (conf_str.substr(0, 4) != "auto") {
                    try {
                        configured_ram = std::stoull(conf_str);
                        is_auto = false;
                    } catch (...) {}
                }
            }
            free(conf_data);
        }
        
        uint64_t resolved_config = ramauto_resolve(configured_ram);
        
        console_write_line(console, "Host RAM\n");
        console_write_line(console, ramauto_to_string(host_ram));
        console_write_line(console, "\nCurrent Virtual RAM\n");
        console_write_line(console, ramauto_to_string(current_virtual_ram));
        console_write_line(console, "\nConfigured Virtual RAM\n");
        console_write_line(console, ramauto_to_string(resolved_config));
        
        console_write_line(console, "\nMode\n");
        if (is_auto) {
            console_write_line(console, "Automatic (1/64 Host RAM)");
        } else {
            console_write_line(console, "Manual");
        }
        
        console_write_line(console, "\nStatus\n");
        if (current_virtual_ram == resolved_config && 
            ((is_auto && current_virtual_ram == ramauto_resolve(TVM_RAM_AUTO)) || (!is_auto))) {
            console_write_line(console, "Active");
        } else {
            console_write_line(console, "Pending Restart");
        }
    } else {
        std::string err = "Unknown command: " + cmd.program;
        console_write_line(console, err.c_str());
    }
}
