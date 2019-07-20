/*******************************************************************************
 * random-flux-loop-teensy/src/neopixel.cpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#include <Arduino.h>

#include <BlinkenAlgorithms/Porting/Teensy.hpp>

#include <BlinkenAlgorithms/Animation/Flux.hpp>
#include <BlinkenAlgorithms/RunAnimation.hpp>
#include <BlinkenAlgorithms/Strip/OctoSK6812Adapter.hpp>

using namespace BlinkenAlgorithms;

/******************************************************************************/

static const size_t strip_size = 300;

DMAMEM int displayMemory[strip_size * 8];
int drawMemory[strip_size * 8];

OctoSK6812 strip(strip_size, displayMemory, drawMemory, SK6812_GRBW);

using MyStrip = OctoSK6812Adapter<OctoSK6812>;
MyStrip my_strip(strip, /* active_parts */ 1);

/******************************************************************************/

bool g_terminate = false;

void delay_poll() { }

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

void loop() {
    static const size_t time_limit = 200000;

    while (1) {
        for (size_t i = 0; i < my_strip.size(); ++i)
            my_strip.setPixel(i, 0);

        size_t a = random(10);
        switch (a) {
        case 0:
            RunAnimation(
                ColorWipeRGBW<MyStrip>(my_strip),
                time_limit);
            break;
        case 1:
            RunAnimation(
                ColorWipeTwoSine<MyStrip>(my_strip),
                time_limit);
            break;
        case 2:
            RunAnimation(
                WheelColorTest<MyStrip>(my_strip),
                time_limit);
            break;
        case 3:
            RunAnimation(
                HSVColorTest<MyStrip>(my_strip),
                time_limit);
            break;
        case 4:
            RunAnimation(
                SparkleWhite<MyStrip>(my_strip),
                time_limit);
            break;
        case 5:
            RunAnimation(
                SparkleRGB<MyStrip>(my_strip),
                time_limit);
            break;
        case 6:
            RunAnimation(
                SparkleWhite<MyStrip>(my_strip, /* speed */ 2000, /* density */ 5),
                time_limit);
            break;
        case 7:
            RunAnimation(
                SparkleRGB<MyStrip>(my_strip, /* speed */ 2000, /* density */ 5),
                time_limit);
            break;
        case 8:
            RunAnimation(
                Fire<MyStrip>(my_strip),
                time_limit);
            break;
        case 9:
            RunAnimation(
                FireIce<MyStrip>(my_strip),
                time_limit);
            break;
        case 10:
            RunAnimation(
                SprayColor<MyStrip>(my_strip, /* reverse */ false),
                time_limit);
            break;
        case 11:
            RunAnimation(
                SprayColor<MyStrip>(my_strip, /* reverse */ true),
                time_limit);
            break;
        case 12:
            RunAnimation(
                Fireworks<MyStrip>(my_strip),
                time_limit);
            break;
        case 13:
            RunAnimation(
                Starlight<MyStrip>(my_strip),
                time_limit);
            break;
        }
    }
}

/******************************************************************************/
