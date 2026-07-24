#pragma once
// ============================================================
// selection.h  –  Text selection in the console
// ============================================================
// A selection is a (line, column) anchor + cursor pair where
// column is the byte-index within the flattened line text.
//
// Normalised access is provided via selStart()/selEnd().
// ============================================================

#include <cstddef>
#include <algorithm>
#include <utility>

struct SelectionPos {
    std::size_t line = 0;
    std::size_t col  = 0;

    bool operator<(const SelectionPos& o) const {
        return (line < o.line) || (line == o.line && col < o.col);
    }
    bool operator==(const SelectionPos& o) const {
        return line == o.line && col == o.col;
    }
};

struct Selection {
    bool active   = false;
    bool dragging = false;

    SelectionPos anchor;
    SelectionPos cursor;

    // ---- mutation --------------------------------------------

    void begin(SelectionPos pos) {
        anchor   = pos;
        cursor   = pos;
        active   = false;   // not active until cursor moves
        dragging = true;
    }

    void update(SelectionPos pos) {
        if (!dragging) return;
        cursor = pos;
        active = !(anchor == cursor);
    }

    void finish(SelectionPos pos) {
        cursor   = pos;
        dragging = false;
        active   = !(anchor == cursor);
    }

    void selectWord(std::size_t line, std::size_t col,
                    const std::string& lineText) {
        if (lineText.empty()) return;
        std::size_t start = col, end = col;
        while (start > 0 && !std::isspace((unsigned char)lineText[start-1])) --start;
        while (end < lineText.size() && !std::isspace((unsigned char)lineText[end])) ++end;
        anchor   = {line, start};
        cursor   = {line, end};
        active   = (start != end);
        dragging = false;
    }

    void selectLine(std::size_t line, const std::string& lineText) {
        anchor   = {line, 0};
        cursor   = {line, lineText.size()};
        active   = !lineText.empty();
        dragging = false;
    }

    void clear() {
        active   = false;
        dragging = false;
    }

    // ---- queries ---------------------------------------------

    SelectionPos selStart() const { return (anchor < cursor) ? anchor : cursor; }
    SelectionPos selEnd()   const { return (anchor < cursor) ? cursor : anchor; }

    // Is column col on line line inside the selection?
    bool contains(std::size_t line, std::size_t col) const {
        if (!active) return false;
        SelectionPos s = selStart(), e = selEnd();
        if (line < s.line || line > e.line) return false;
        if (s.line == e.line) return col >= s.col && col < e.col;
        if (line == s.line)   return col >= s.col;
        if (line == e.line)   return col < e.col;
        return true; // fully covered middle line
    }
};
