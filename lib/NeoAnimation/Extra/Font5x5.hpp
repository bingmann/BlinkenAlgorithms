/*******************************************************************************
 * lib/NeoAnimation/Extra/Font5x5.hpp
 *
 * Simple 5 pixel high dot matrix font for MAX7219
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#ifndef NEOANIMATION_EXTRA_FONT5X5_HEADER
#define NEOANIMATION_EXTRA_FONT5X5_HEADER

#include <iostream>

#include <NeoAnimation/Extra/MAX7219.hpp>

namespace NeoAnimation {

class Font5x5
{
public:
    //! Load font
    Font5x5();

    uint8_t find_char(char letter);
    void print(const char* str, MAX7219& led_matrix);
    void print_right(const char* str, MAX7219& led_matrix);

protected:
    static const size_t num_glyphs = 37;

    static char characters_[num_glyphs];

    uint8_t dots_[num_glyphs][5][5];
    uint8_t widths_[num_glyphs];
};

char Font5x5::characters_[num_glyphs] = {
    ' ',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'
};

#include "Font5x5.xpm"

Font5x5::Font5x5() {
    size_t j = 0;

    // first letter is space (1px width)
    for (size_t x = 0; x < 5; ++x) {
        for (size_t y = 0; y < 5; ++y) {
            dots_[j][x][y] = 0;
        }
    }
    widths_[j] = 1;
    ++j;

    // find characters_ of black and white color
    unsigned width, height, colors, cpp;
    int x = sscanf(Font5x5_xpm[0], "%u %u %u %u",
                   &width, &height, &colors, &cpp);
    if (x != 4) {
        std::cout << "Invalid XPM format?" << std::endl;
        return;
    }

    char black = 0, white = 0;
    for (unsigned i = 0; i < colors; ++i) {
        char c;
        unsigned r, g, b;
        x = sscanf(Font5x5_xpm[i + 1], "%c\tc #%02x%02x%02x",
                   &c, &r, &g, &b);
        if (r == 0 && g == 0 && b == 0)
            black = c;
        if (r == 255 && g == 255 && b == 255)
            white = c;
    }

    if (black == 0 || white == 0) {
        std::cout << "Invalid XPM format? no black or white" << std::endl;
        return;
    }

    const char* r0 = Font5x5_xpm[1 + colors + 0];
    const char* r1 = Font5x5_xpm[1 + colors + 1];
    const char* r2 = Font5x5_xpm[1 + colors + 2];
    const char* r3 = Font5x5_xpm[1 + colors + 3];
    const char* r4 = Font5x5_xpm[1 + colors + 4];

    while (j < num_glyphs) {
        size_t w = 0;
        while (*r0 != white || *r1 != white || *r2 != white ||
               *r3 != white || *r4 != white)
        {
            dots_[j][w][0] |= (*r0 == black ? 1 : 0);
            dots_[j][w][1] |= (*r1 == black ? 1 : 0);
            dots_[j][w][2] |= (*r2 == black ? 1 : 0);
            dots_[j][w][3] |= (*r3 == black ? 1 : 0);
            dots_[j][w][4] |= (*r4 == black ? 1 : 0);

            ++w;
            ++r0, ++r1, ++r2, ++r3, ++r4;
        }

        ++r0, ++r1, ++r2, ++r3, ++r4;
        widths_[j] = w;
        ++j;
    }
}

uint8_t Font5x5::find_char(char letter) {
    for (uint8_t index = 0; index < num_glyphs; index++) {
        if (letter == characters_[index]) {
            return index;
        }
    }
    // Character not found, return index of space
    return 0;
}

void Font5x5::print(const char* str, MAX7219& led_matrix) {
    size_t a = 0, b = 0;
    for (size_t i = 0; str[i]; ++i) {
        char letter1 = toupper(str[i]);
        if (letter1 == '\n') {
            a = 0, b += 6;
            continue;
        }

        uint8_t c = find_char(letter1);

        if (a + widths_[c] >= 8 * 8) {
            a = 0, b += 6;
        }

        for (size_t x = 0; x < widths_[c]; ++x) {
            for (size_t y = 0; y < 5; ++y) {
                led_matrix.PutPixel(a, b + y, dots_[c][x][y]);
            }
            ++a;
        }
        for (size_t y = 0; y < 5; ++y) {
            led_matrix.PutPixel(a, b + y, false);
        }
        ++a;
    }
}

void Font5x5::print_right(const char* str, MAX7219& led_matrix) {
    size_t a = 8 * 8, b = 2 * 8 - 5, slen = strlen(str);
    for (size_t i = slen; i != 0; ) {
        --i;
        char letter1 = toupper(str[i]);

        uint8_t c = find_char(letter1);

        a -= 1 + widths_[c];
        for (size_t y = 0; y < 5; ++y) {
            led_matrix.PutPixel(a, b + y, false);
        }
        for (size_t x = 0; x < widths_[c]; ++x) {
            for (size_t y = 0; y < 5; ++y) {
                led_matrix.PutPixel(a + 1 + x, b + y, dots_[c][x][y]);
            }
        }
    }
}

} // namespace NeoAnimation

#endif // !NEOANIMATION_EXTRA_FONT5X5_HEADER

/******************************************************************************/
