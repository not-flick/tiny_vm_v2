#include "console.h"
#include <cstring>
#include <cstdlib>

extern "C" {

ConsoleHandle* console_create(int width, int height, const char* title) {
    return reinterpret_cast<ConsoleHandle*>(new Console(width, height, title));
}

void console_destroy(ConsoleHandle* console) {
    delete reinterpret_cast<Console*>(console);
}

void console_write(ConsoleHandle* console, const char* text) {
    if (console && text) {
        reinterpret_cast<Console*>(console)->write(text);
    }
}

void console_write_line(ConsoleHandle* console, const char* text) {
    if (console && text) {
        reinterpret_cast<Console*>(console)->writeLine(text);
    }
}

bool console_write_image(ConsoleHandle* console, const char* path) {
    if (console && path) {
        return reinterpret_cast<Console*>(console)->writeImage(path);
    }
    return false;
}

char* console_read_line(ConsoleHandle* console) {

    if (console) {
        std::string str = reinterpret_cast<Console*>(console)->readLine();
        char* cstr = (char*)malloc(str.length() + 1);
        if (cstr) {
            std::strcpy(cstr, str.c_str());
            return cstr;
        }
    }
    return nullptr;
}

void console_clear(ConsoleHandle* console) {
    if (console) {
        reinterpret_cast<Console*>(console)->clear();
    }
}

void console_resize(ConsoleHandle* console, int width, int height) {
    if (console) {
        reinterpret_cast<Console*>(console)->resize(width, height);
    }
}

void console_set_title(ConsoleHandle* console, const char* title) {
    if (console && title) {
        reinterpret_cast<Console*>(console)->setTitle(title);
    }
}

void console_close(ConsoleHandle* console) {
    if (console) {
        reinterpret_cast<Console*>(console)->close();
    }
}

bool console_is_open(ConsoleHandle* console) {
    if (console) {
        return reinterpret_cast<Console*>(console)->isOpen();
    }
    return false;
}

void console_poll_events(ConsoleHandle* console) {
    if (console) {
        reinterpret_cast<Console*>(console)->pollEvents();
    }
}

void console_present(ConsoleHandle* console) {
    if (console) {
        reinterpret_cast<Console*>(console)->present();
    }
}

bool console_load_font(ConsoleHandle* console, const char* path, float size) {
    if (console && path) {
        return reinterpret_cast<Console*>(console)->loadFont(path, size);
    }
    return false;
}

bool console_load_default_font(ConsoleHandle* console, float size) {
    if (console) {
        return reinterpret_cast<Console*>(console)->loadDefaultFont(size);
    }
    return false;
}

void console_unload_font(ConsoleHandle* console) {
    if (console) {
        reinterpret_cast<Console*>(console)->unloadFont();
    }
}

bool console_has_font(ConsoleHandle* console) {
    if (console) {
        return reinterpret_cast<Console*>(console)->hasFont();
    }
    return false;
}

float console_font_size(ConsoleHandle* console) {
    if (console) {
        return reinterpret_cast<Console*>(console)->fontSize();
    }
    return 0.0f;
}

} // extern "C"
