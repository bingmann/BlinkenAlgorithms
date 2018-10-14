/*******************************************************************************
 * lib/NeoAnimation/RunAnimation.hpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#ifndef NEOANIMATION_RUNANIMATION_HEADER
#define NEOANIMATION_RUNANIMATION_HEADER

#include <algorithm>
#include <cstdint>

extern bool g_terminate;

namespace NeoAnimation {

// special return values for animations
static const uint32_t EndAnimation = uint32_t(-1);
static const uint32_t NoUpdate = uint32_t(-2);

/******************************************************************************/

template <template <class> class Animation,
          typename LEDStrip>
void RunAnimation(LEDStrip& strip1,
                  size_t time_limit) {
    Animation<LEDStrip> ani1(strip1);
    uint32_t ts_end = micros() + 1000 * time_limit;
    g_terminate = false;

    for (uint32_t s = 0; micros() < ts_end; ++s) {
        uint32_t d = ani1(strip1, s);

        if (d == EndAnimation)
            break;
        else if (d == NoUpdate)
            d = 0;
        else {
            if (!strip1.busy()) {
                strip1.show();
            }
        }

        delay_micros(d);

        if (g_terminate)
            break;
    }
}

template <template <class> class Animation1,
          template <class> class Animation2,
          typename LEDStrip1,
          typename LEDStrip2>
void RunAnimation(LEDStrip1& strip1, LEDStrip2& strip2,
                  size_t time_limit) {
    Animation1<LEDStrip1> ani1(strip1);
    Animation2<LEDStrip2> ani2(strip2);
    uint32_t ts_end = micros() + 1000 * time_limit;
    uint32_t s1 = 0, s2 = 0;
    uint32_t d1 = 0, d2 = 0;
    bool dirty1 = false, dirty2 = false;

    g_terminate = false;

    while (true) {
        uint32_t ts = micros();
        if (ts_end < ts)
            break;

        if (d1 < ts) {
            uint32_t d = ani1(strip1, s1++);
            if (d == EndAnimation) {
                d1 = EndAnimation;
            }
            else if (d == NoUpdate) { }
            else {
                d1 += d;
                dirty1 = true;
            }
        }
        if (d2 < ts) {
            uint32_t d = ani2(strip2, s2++);
            if (d == EndAnimation) {
                d2 = EndAnimation;
            }
            else if (d == NoUpdate) { }
            else {
                d2 += d;
                dirty2 = true;
            }
        }

        if (dirty1 && !strip1.busy()) {
            strip1.show();
            dirty1 = false;
        }
        if (dirty2 && !strip2.busy()) {
            strip2.show();
            dirty2 = false;
        }

        uint32_t d = std::min(d1, d2);
        if (d >= ts)
            delay_micros(d - ts);

        if (d1 == EndAnimation && d2 == EndAnimation)
            break;

        if (g_terminate)
            break;
    }
}

template <template <class> class Animation1,
          template <class> class Animation2,
          template <class> class Animation3,
          typename LEDStrip1,
          typename LEDStrip2,
          typename LEDStrip3>
void RunAnimation(LEDStrip1& strip1, LEDStrip2& strip2,
                  LEDStrip2& strip3,
                  size_t time_limit) {
    Animation1<LEDStrip1> ani1(strip1);
    Animation2<LEDStrip2> ani2(strip2);
    Animation3<LEDStrip3> ani3(strip3);
    uint32_t ts_end = micros() + 1000 * time_limit;
    uint32_t s1 = 0, s2 = 0, s3 = 0;
    uint32_t d1 = 0, d2 = 0, d3 = 0;
    bool dirty1 = false, dirty2 = false, dirty3 = false;

    g_terminate = false;

    while (true) {
        uint32_t ts = micros();
        if (ts_end < ts)
            break;

        if (d1 < ts) {
            uint32_t d = ani1(strip1, s1++);
            if (d == EndAnimation) {
                d1 = EndAnimation;
            }
            else if (d == NoUpdate) { }
            else {
                d1 += d;
                dirty1 = true;
            }
        }
        if (d2 < ts) {
            uint32_t d = ani2(strip2, s2++);
            if (d == EndAnimation) {
                d2 = EndAnimation;
            }
            else if (d == NoUpdate) { }
            else {
                d2 += d;
                dirty2 = true;
            }
        }
        if (d3 < ts) {
            uint32_t d = ani3(strip3, s3++);
            if (d == EndAnimation) {
                d3 = EndAnimation;
            }
            else if (d == NoUpdate) { }
            else {
                d3 += d;
                dirty3 = true;
            }
        }

        if (dirty1 && !strip1.busy()) {
            strip1.show();
            dirty1 = false;
        }
        if (dirty2 && !strip2.busy()) {
            strip2.show();
            dirty2 = false;
        }
        if (dirty3 && !strip3.busy()) {
            strip3.show();
            dirty3 = false;
        }

        uint32_t d = std::min(std::min(d1, d2), d3);
        if (d >= ts)
            strip1.micros(d - ts);

        if (d1 == EndAnimation && d2 == EndAnimation &&
            d3 == EndAnimation)
            break;

        if (g_terminate)
            break;
    }
}

} // namespace NeoAnimation

#endif // !NEOANIMATION_RUNANIMATION_HEADER

/******************************************************************************/
