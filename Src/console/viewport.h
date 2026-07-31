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
    // Number of entries scrolled up from the bottom (0 = pinned to bottom).
    std::size_t scrollOffset = 0;

    // When true, new output automatically keeps the view at the bottom.
    bool autoFollow = true;

    void scrollUp(std::size_t lines, std::size_t totalLines) {
        if (totalLines > 0) {
            std::size_t limit = totalLines - 1;
            scrollOffset = std::min(scrollOffset + lines, limit);
            autoFollow = (scrollOffset == 0);
        }
    }

    void scrollDown(std::size_t lines) {
        if (scrollOffset <= lines) {
            scrollOffset = 0;
            autoFollow = true;
        } else {
            scrollOffset -= lines;
        }
    }

    void pageUp(std::size_t totalLines, std::size_t visibleLinesEst) {
        scrollUp(visibleLinesEst, totalLines);
    }

    void pageDown(std::size_t visibleLinesEst) {
        scrollDown(visibleLinesEst);
    }

    void goHome(std::size_t totalLines) {
        if (totalLines > 0) {
            scrollOffset = totalLines - 1;
            autoFollow = false;
        }
    }

    void goEnd() {
        scrollOffset = 0;
        autoFollow = true;
    }

    void onNewLine() {
        if (autoFollow) {
            scrollOffset = 0;
        }
        // If not following, we technically stay at the same scrollOffset (which means
        // the view moves UP with the old content, because the old content is now
        // further from the bottom).
    }
};

