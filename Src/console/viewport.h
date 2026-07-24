#pragma once
// ============================================================
// viewport.h  –  Viewport over the scrollback buffer
// ============================================================
// scrollOffset == 0   → bottom (newest output visible).
// scrollOffset == N   → N lines scrolled up from the bottom.
// autoFollow == true  → new output keeps the view pinned to
//                       the bottom.
// ============================================================

#include <cstddef>
#include <algorithm>

class ScrollbackBuffer;   // forward decl – defined in scrollback.h

struct Viewport {
    // Lines scrolled up from the bottom (0 = pinned to bottom).
    std::size_t scrollOffset = 0;

    // When true, new output automatically keeps the view at
    // the bottom.  Cleared when the user scrolls up manually.
    bool autoFollow = true;

    // ---- helpers ---------------------------------------------

    // Maximum valid scrollOffset so that at least one page of
    // lines is visible.
    static std::size_t maxOffset(std::size_t totalLines,
                                 std::size_t visibleLines) {
        if (totalLines <= visibleLines) return 0;
        return totalLines - visibleLines;
    }

    // Index of the first visible line in the scrollback buffer.
    std::size_t firstVisible(std::size_t totalLines,
                             std::size_t visibleLines) const {
        std::size_t bottom = (totalLines > visibleLines)
                             ? totalLines - visibleLines
                             : 0;
        std::size_t top = (bottom >= scrollOffset)
                          ? bottom - scrollOffset
                          : 0;
        return top;
    }

    // ---- scroll actions --------------------------------------

    void scrollUp(std::size_t lines, std::size_t totalLines,
                  std::size_t visibleLines) {
        std::size_t limit = maxOffset(totalLines, visibleLines);
        scrollOffset = std::min(scrollOffset + lines, limit);
        autoFollow   = (scrollOffset == 0);
    }

    void scrollDown(std::size_t lines) {
        if (scrollOffset <= lines) {
            scrollOffset = 0;
            autoFollow   = true;
        } else {
            scrollOffset -= lines;
        }
    }

    void pageUp(std::size_t totalLines, std::size_t visibleLines) {
        scrollUp(visibleLines, totalLines, visibleLines);
    }

    void pageDown(std::size_t visibleLines) {
        scrollDown(visibleLines);
    }

    void goHome(std::size_t totalLines, std::size_t visibleLines) {
        scrollOffset = maxOffset(totalLines, visibleLines);
        autoFollow   = false;
    }

    void goEnd() {
        scrollOffset = 0;
        autoFollow   = true;
    }

    // Called after new lines are pushed to follow if pinned.
    void onNewLine(std::size_t totalLines, std::size_t visibleLines) {
        if (autoFollow) {
            scrollOffset = 0;
        } else {
            // Keep the same absolute position in history.
            std::size_t limit = maxOffset(totalLines, visibleLines);
            if (scrollOffset > limit) scrollOffset = limit;
        }
    }
};
