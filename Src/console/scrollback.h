#pragma once
// ============================================================
// scrollback.h  –  Circular scrollback line buffer
// ============================================================
// Stores TextLine objects in a fixed-capacity ring buffer.
// When the buffer is full the oldest line is silently discarded.
// Access is always O(1); iteration for rendering uses the
// firstVisible / lastVisible range so we never touch the full
// history on every frame.
// ============================================================

#include <cstddef>
#include <string>
#include <vector>

// Forward-declare the segment type used throughout the console.
// The full definition lives in console.h which includes this file.
struct TextSegment {
    std::string text;
    uint8_t r = 255;
    uint8_t g = 255;
    uint8_t b = 255;
};
using TextLine = std::vector<TextSegment>;

class ScrollbackBuffer {
public:
    // ---- construction ----------------------------------------
    explicit ScrollbackBuffer(std::size_t capacity = 10000)
        : mCapacity(capacity)
        , mLines(capacity)
    {}

    // ---- mutation --------------------------------------------

    // Append a fully formed line.  Never fails – oldest line is
    // silently discarded when the buffer is at capacity.
    void pushLine(const TextLine& line) {
        if (mSize < mCapacity) {
            mLines[(mStart + mSize) % mCapacity] = line;
            ++mSize;
        } else {
            mLines[mStart] = line;
            mStart = (mStart + 1) % mCapacity;
        }
    }

    // Replace the last line in-place (used while building an
    // incomplete line that hasn't received a newline yet).
    void replaceLastLine(const TextLine& line) {
        if (mSize == 0) { pushLine(line); return; }
        mLines[(mStart + mSize - 1) % mCapacity] = line;
    }

    // Access the last stored line for appending segments.
    TextLine& lastLine() {
        if (mSize == 0) { pushLine({}); }
        return mLines[(mStart + mSize - 1) % mCapacity];
    }

    void clear() {
        mStart = 0;
        mSize  = 0;
    }

    // ---- queries ---------------------------------------------

    // Random access by logical index (0 = oldest, size()-1 = newest).
    const TextLine& lineAt(std::size_t idx) const {
        return mLines[(mStart + idx) % mCapacity];
    }

    std::size_t size()     const { return mSize; }
    std::size_t capacity() const { return mCapacity; }

    // ---- resize ----------------------------------------------

    void setCapacity(std::size_t newCap) {
        if (newCap == mCapacity) return;
        std::vector<TextLine> tmp(newCap);
        std::size_t keep = (mSize < newCap) ? mSize : newCap;
        for (std::size_t i = 0; i < keep; ++i)
            tmp[i] = lineAt(mSize - keep + i);   // keep the newest lines
        mLines.swap(tmp);
        mStart    = 0;
        mSize     = keep;
        mCapacity = newCap;
    }

private:
    std::size_t           mCapacity;
    std::vector<TextLine> mLines;
    std::size_t           mStart = 0;  // index of the oldest line
    std::size_t           mSize  = 0;  // number of valid lines
};
