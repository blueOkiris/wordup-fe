// Application global constants

#pragma once

#include <imgui.h>
#include <subwin.hpp>

namespace global {
    static const size_t width = 1280;
    static const size_t height = 720;
    static const char *const title = "Wordup FE";
    static const int fps = 60;
    static const ImWchar ipaRanges[] = {
        0x0020, 0x00FF, // Basic Latin + Latin-1 Supplement
        0x0100, 0x017F, // Latin Extended-A
        0x0180, 0x024F, // Latin Extended-B
        0x0250, 0x02AF, // IPA Extensions
        0x02B0, 0x02FF, // Spacing Modifier Letters
        0x0300, 0x036F, // Combining Diacritical Marks
        0x1D00, 0x1D7F, // Phonetic Extensions
        0x1D80, 0x1DBF, // Phonetic Extensions Supplement
        0
    };

    extern ImFont *fontCharisSil;
    extern bool firstLaunch;
}

