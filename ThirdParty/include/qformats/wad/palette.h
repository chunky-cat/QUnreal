#pragma once

#include <vector>
#include "default_pal.h"

namespace qformats::wad
{
    struct color
    {
        u_char rgba[4] = {0, 0, 0, 1};
    };

    using cvec = std::vector<color>;
    // 240
    class Palette
    {
    public:
        static Palette FromFile(const std::string &fileName);
        static Palette FromBuffer(const unsigned char *buff, int size);
        color GetColor(int index);
        cvec GetBrightColors();

    private:
        cvec colors;
    };
    static Palette default_palette = Palette::FromBuffer(default_palette_lmp, default_palette_size);
}