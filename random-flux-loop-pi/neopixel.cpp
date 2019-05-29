/*******************************************************************************
 * random-flux-loop-pi/neopixel.cpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#include <NeoAnimation/Porting/RaspberryPi.hpp>

#include <NeoAnimation/Animation/Flux.hpp>
#include <NeoAnimation/RunAnimation.hpp>
#include <NeoAnimation/Strip/PiSPI_APA102.hpp>

using namespace NeoAnimation;

using MyStrip = PiSPI_APA102;
MyStrip my_strip("/dev/spidev0.0", /* strip_size */ 5 * 96);

void delay_poll() { }

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
                HSVColorWheel<MyStrip>(my_strip),
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
                Fire<MyStrip>(my_strip),
                time_limit);
            break;
        case 7:
            RunAnimation(
                FireIce<MyStrip>(my_strip),
                time_limit);
            break;
        case 8:
            RunAnimation(
                SprayColor<MyStrip>(my_strip),
                time_limit);
            break;
        case 9:
            RunAnimation(
                Fireworks<MyStrip>(my_strip),
                time_limit);
            break;
        }
    }

    return 0;
}

/******************************************************************************/
