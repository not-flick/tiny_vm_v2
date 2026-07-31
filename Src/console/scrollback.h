#pragma once
// ============================================================
// scrollback.h  –  Circular scrollback buffer
// ============================================================
// Stores ConsoleEntry objects (Text or Image) in a fixed-capacity 
// ring buffer. When the buffer is full the oldest entry is silently 
// discarded.
// ============================================================

#include <cstddef>
#include <string>
#include <vector>

struct SDL_Texture;

// Forward-declare the segment type used throughout the console.
struct TextSegment {
    std::string text;
    uint8_t r = 255;
    uint8_t g = 255;
    uint8_t b = 255;
};
using TextLine = std::vector<TextSegment>;

struct ConsoleEntry {
    enum class Type { Text, Image };
    Type type = Type::Text;
    
    // For Text
    TextLine textLine;
    
    // For Image
    SDL_Texture* texture = nullptr;
    int imageWidth = 0;
    int imageHeight = 0;
};

class ScrollbackBuffer {
public:
    // ---- construction ----------------------------------------
    explicit ScrollbackBuffer(std::size_t capacity = 10000)
        : mCapacity(capacity)
        , mEntries(capacity)
    {}

    // ---- mutation --------------------------------------------

    void pushEntry(const ConsoleEntry& entry) {
        if (mSize < mCapacity) {
            mEntries[(mStart + mSize) % mCapacity] = entry;
            ++mSize;
        } else {
            mEntries[mStart] = entry;
            mStart = (mStart + 1) % mCapacity;
        }
    }

    void pushLine(const TextLine& line) {
        ConsoleEntry entry;
        entry.type = ConsoleEntry::Type::Text;
        entry.textLine = line;
        pushEntry(entry);
    }

    void replaceLastLine(const TextLine& line) {
        if (mSize == 0) { pushLine(line); return; }
        std::size_t idx = (mStart + mSize - 1) % mCapacity;
        if (mEntries[idx].type == ConsoleEntry::Type::Text) {
            mEntries[idx].textLine = line;
        } else {
            pushLine(line);
        }
    }

    TextLine& lastLine() {
        if (mSize == 0) { pushLine({}); }
        std::size_t idx = (mStart + mSize - 1) % mCapacity;
        if (mEntries[idx].type != ConsoleEntry::Type::Text) {
            pushLine({});
            idx = (mStart + mSize - 1) % mCapacity;
        }
        return mEntries[idx].textLine;
    }

    void clear() {
        mStart = 0;
        mSize  = 0;
    }

    // ---- queries ---------------------------------------------

    const ConsoleEntry& entryAt(std::size_t idx) const {
        return mEntries[(mStart + idx) % mCapacity];
    }
    
    // Legacy support, returns empty if not text
    const TextLine& lineAt(std::size_t idx) const {
        return mEntries[(mStart + idx) % mCapacity].textLine;
    }

    std::size_t size()     const { return mSize; }
    std::size_t capacity() const { return mCapacity; }

    // ---- resize ----------------------------------------------

    void setCapacity(std::size_t newCap) {
        if (newCap == mCapacity) return;
        std::vector<ConsoleEntry> tmp(newCap);
        std::size_t keep = (mSize < newCap) ? mSize : newCap;
        for (std::size_t i = 0; i < keep; ++i)
            tmp[i] = entryAt(mSize - keep + i);
        mEntries.swap(tmp);
        mStart    = 0;
        mSize     = keep;
        mCapacity = newCap;
    }

private:
    std::size_t               mCapacity;
    std::vector<ConsoleEntry> mEntries;
    std::size_t               mStart = 0;  // index of the oldest entry
    std::size_t               mSize  = 0;  // number of valid entries
};
