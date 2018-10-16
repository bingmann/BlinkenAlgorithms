/*******************************************************************************
 * random-flux-loop-teensy/src/neopixel.cpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#include <Arduino.h>
#include <NeoAnimation/Porting/Teensy.hpp>

/******************************************************************************/

#include <NeoAnimation/Animation/Flux.hpp>
#include <NeoAnimation/RunAnimation.hpp>
#include <NeoAnimation/Strip/OctoSK6812Adapter.hpp>

static const size_t strip_size = 300;

DMAMEM int displayMemory[strip_size * 8];
int drawMemory[strip_size * 8];

OctoSK6812 strip(strip_size, displayMemory, drawMemory, SK6812_GRBW);

using SK6812 = NeoAnimation::OctoSK6812Adapter<OctoSK6812>;
SK6812 my_strip(strip);

/******************************************************************************/

bool g_terminate = false;

void setup() {
    Serial.begin(115200);
    Serial.println("Starting...");

    unsigned long seed = 0;
    for (int i = 0; i < 32; i++) {
        seed ^= ((analogRead(A0) & 0x01) << i);
    }
    randomSeed(seed);
    srandom(seed);

    strip.begin();
}

using namespace NeoAnimation;

void loop() {
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
            RunAnimation<HSVColorTest>(my_strip, time_limit);
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
}

/******************************************************************************/
