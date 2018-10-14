/*******************************************************************************
 * random-flux-loop-pi/neopixel.cpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#include <cstdint>
#include <cstdlib>

// add missing global functions
uint32_t random(uint32_t limit) {
    return ::random() % limit;
}

uint32_t random(uint32_t begin, uint32_t limit) {
    return ::random() % (limit - begin) + begin;
}

#include <NeoAnimation/Animation/Flux.hpp>
#include <NeoAnimation/RunAnimation.hpp>
#include <NeoAnimation/Strip/PiSPI_APA102.hpp>

using namespace NeoAnimation;

PiSPI_APA102 my_strip("/dev/spidev0.0", /* strip_size */ 5 * 96);

bool g_terminate = false;

int main() {
    srandom(time(nullptr));

    static const size_t time_limit = 20000;

    while (1) {
        for (size_t i = 0; i < my_strip.size(); ++i)
            my_strip.setPixel(i, 0);

        size_t a = random(10);
        switch (a) {
        case 0:
            RunAnimation<ColorWipeRGBW>(my_strip, time_limit);
            break;
        case 1:
            RunAnimation<ColorWipeTwoSine>(my_strip, time_limit);
            break;
        case 2:
            RunAnimation<WheelColorTest>(my_strip, time_limit);
            break;
        case 3:
            RunAnimation<HSVColorWheel>(my_strip, time_limit);
            break;
        case 4:
            RunAnimation<SparkleWhite>(my_strip, time_limit);
            break;
        case 5:
            RunAnimation<SparkleRGB>(my_strip, time_limit);
            break;
        case 6:
            RunAnimation<Fire>(my_strip, time_limit);
            break;
        case 7:
            RunAnimation<FireIce>(my_strip, time_limit);
            break;
        case 8:
            RunAnimation<SprayColor>(my_strip, time_limit);
            break;
        case 9:
            RunAnimation<Fireworks>(my_strip, time_limit);
            break;
        }
    }

    return 0;
}

/******************************************************************************/
