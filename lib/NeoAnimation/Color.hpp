/*******************************************************************************
 * lib/NeoAnimation/Color.hpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#ifndef NEOANIMATION_COLOR_HEADER
#define NEOANIMATION_COLOR_HEADER

#include <algorithm>
#include <cstdint>

namespace NeoAnimation {

// Color union for WBGR strips
struct Color {
    union {
        struct {
            uint8_t w;
            uint8_t b;
            uint8_t g;
            uint8_t r;
        } __attribute__ ((packed));
        struct {
            uint8_t white;
            uint8_t blue;
            uint8_t green;
            uint8_t red;
        } __attribute__ ((packed));
        uint32_t v;
    } __attribute__ ((packed));

    Color() = default;

    Color(uint8_t w) : w(w), b(0), g(0), r(0) { }

    Color(uint8_t r, uint8_t g, uint8_t b) : w(0), b(b), g(g), r(r) { }

    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t w)
        : w(w), b(b), g(g), r(r) { }
};

//! Input a value 0 to 255 to get a color value.
//! The colours are a transition r - g - b - back to r.
static inline Color WheelColor(uint32_t i, uint8_t intensity) {
    if (intensity == 0)
        return Color(0);
    i &= 0xFF;
    if (i < 85) {
        return Color(
            (256 - i * 3) * intensity / 255u, 0, (i * 3) * intensity / 255u);
    }
    if (i < 170) {
        i -= 85;
        return Color(
            0, (i * 3) * intensity / 255u, (255 - i * 3) * intensity / 255u);
    }
    i -= 170;
    return Color(
        (i * 3) * intensity / 255u, (255 - i * 3) * intensity / 255u, 0);
}

//! Input a value 0 to 255 to get a color value.
//! The colours are a transition r - g - b - w.
static inline Color WheelColorWhite(uint32_t i, uint8_t intensity) {
    if (intensity == 0)
        return Color(0);
    i &= 0xFF;
    if (i < 64) {
        return Color(
            (256 - i * 4) * intensity / 255u, 0, (i * 4) * intensity / 255u);
    }
    if (i < 128) {
        i -= 64;
        return Color(
            0, (i * 4) * intensity / 255u, (255 - i * 4) * intensity / 255u);
    }
    if (i < 192) {
        i -= 128;
        return Color(
            (i * 4) * intensity / 255u, (255 - i * 4) * intensity / 255u, 0);
    }
    i -= 192;
    return Color(
        0, 0, (256 - i * 4) * intensity / 255u, (i * 4) * intensity / 255u);
}

//! hue ranges 0--HSV_HUE_MAX=1535, sat and val range 0-255.
static inline Color HSVColor(uint16_t hue, uint8_t sat, uint8_t val) {
    /*
     * The MIT License (MIT)
     *
     * Copyright (c) 2016  B. Stultiens
     *
     * Permission is hereby granted, free of charge, to any person obtaining a
     * copy of this software and associated documentation files (the
     * "Software"), to deal in the Software without restriction, including
     * without limitation the rights to use, copy, modify, merge, publish,
     * distribute, sublicense, and/or sell copies of the Software, and to permit
     * persons to whom the Software is furnished to do so, subject to the
     * following conditions:
     *
     * The above copyright notice and this permission notice shall be included
     * in all copies or substantial portions of the Software.
     *
     * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
     * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
     * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
     * NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
     * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
     * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
     * USE OR OTHER DEALINGS IN THE SOFTWARE.
     */

#define HSV_HUE_SEXTANT 256
#define HSV_HUE_STEPS (6 * HSV_HUE_SEXTANT)

#define HSV_HUE_MIN 0
#define HSV_HUE_MAX (HSV_HUE_STEPS - 1)
#define HSV_SAT_MIN 0
#define HSV_SAT_MAX 255
#define HSV_VAL_MIN 0
#define HSV_VAL_MAX 255

    uint8_t _r, _g, _b;
    uint8_t* r = &_r, * g = &_g, * b = &_b;

    if (!sat) {
        // Exit with grayscale if sat == 0
        return Color(val, val, val);
    }

    uint8_t sextant = hue >> 8;

    // Optional: Limit hue sextants to defined space
    if (sextant > 5) {
        sextant = 5;
    }

    /*
     * Swap pointers depending which sextant we are in
     *
     * Pointer swapping:
     * -------  sext.   r g b   r<>b    g<>b    r <> g  result
     * 0 0 0    v u c                   !u v c  u v c
     * 0 0 1    d v c                           d v c
     * 0 1 0    c v u   u v c                   u v c
     * 0 1 1    c d v   v d c           d v c   d v c
     * 1 0 0    u c v           u v c           u v c
     * 1 0 1    v c d           v d c   d v c   d v c
     */
    if (sextant & 2) {
        std::swap(r, b);
    }
    if (sextant & 4) {
        std::swap(g, b);
    }
    if (!(sextant & 6)) {
        if (!(sextant & 1)) {
            std::swap(r, g);
        }
    }
    else if (sextant & 1) {
        std::swap(r, g);
    }

    *g = val; // Top level

    // Perform actual calculations

    /*
     * Bottom level: v * (1.0 - s)
     * --> (v * (255 - sat) + error_corr + 1) / 256
     */
    uint16_t ww; // Intermediate result
    ww = val *
         (255 - sat); // We don't use ~s to prevent size-promotion side effects
    ww += 1;          // Error correction
    ww += ww >> 8;    // Error correction
    *b = ww >> 8;

    uint8_t h_fraction = hue & 0xff; // 0...255
    uint32_t d;                      // Intermediate result

    if (!(sextant & 1)) {
        // *r = ...slope_up...;
        d = val * (uint32_t)((255 << 8) - (uint16_t)(sat * (256 - h_fraction)));
        d += d >> 8; // Error correction
        d += val;    // Error correction
        *r = d >> 16;
    }
    else {
        // *r = ...slope_down...;
        d = val * (uint32_t)((255 << 8) - (uint16_t)(sat * h_fraction));
        d += d >> 8; // Error correction
        d += val;    // Error correction
        *r = d >> 16;
    }

    return Color(_r, _g, _b);
}

} // namespace NeoAnimation

#endif // !NEOANIMATION_COLOR_HEADER

/******************************************************************************/
