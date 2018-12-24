/*******************************************************************************
 * random-flux-loop-esp8266/src/neopixel.cpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#include <Arduino.h>

#include <NeoAnimation/Animation/Flux.hpp>
#include <NeoAnimation/RunAnimation.hpp>
#include <NeoAnimation/Strip/NeoPixelBusAdapter.hpp>

// four element pixels, RGBW SK6812 strip
NeoPixelBus<NeoRgbwFeature, NeoEsp8266Dma800KbpsMethod> strip(/* strip_size */ 300);

// APA102 "DotStar" strip
// NeoPixelBus<DotStarBgrFeature, DotStarSpiMethod> strip(/* array_size */ 5 * 96);

/******************************************************************************/

// three element pixels, in different order and speeds
// NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);
// NeoPixelBus<NeoRgbFeature, Neo400KbpsMethod> strip(PixelCount, PixelPin);

// For Esp8266, the Pin is omitted and it uses GPIO3 due to DMA hardware use.
// There are other Esp8266 alternative methods that provide more pin options,
// but also have other side effects.
// NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount);
//
// NeoEsp8266Uart800KbpsMethod uses GPI02 instead

// You can also use one of these for Esp8266,
// each having their own restrictions
//
// These two are the same as above as the DMA method is the default
// NOTE: These will ignore the PIN and use GPI03 pin
// NeoPixelBus<NeoGrbFeature, NeoEsp8266Dma800KbpsMethod> strip(strip_size);
// NeoPixelBus<NeoRgbFeature, NeoEsp8266Dma400KbpsMethod> strip(PixelCount, PixelPin);

// Uart method is good for the Esp-01 or other pin restricted modules
// NOTE: These will ignore the PIN and use GPI02 pin
// NeoPixelBus<NeoGrbFeature, NeoEsp8266Uart800KbpsMethod> strip(PixelCount, PixelPin);
// NeoPixelBus<NeoRgbFeature, NeoEsp8266Uart400KbpsMethod> strip(PixelCount, PixelPin);

// The bitbang method is really only good if you are not using WiFi features of
// the ESP. It works with all but pin 16
// NeoPixelBus<NeoGrbFeature, NeoEsp8266BitBang800KbpsMethod> strip(PixelCount, PixelPin);
// NeoPixelBus<NeoRgbFeature, NeoEsp8266BitBang400KbpsMethod> strip(PixelCount, PixelPin);

bool g_terminate = false;

void delay_poll() { }

void setup() {
    Serial.begin(115200);

    randomSeed(analogRead(0));
    strip.Begin();
}

using namespace NeoAnimation;

void loop() {
    using MyStrip = NeoPixelBusAdapter<decltype(strip)>;
    MyStrip my_strip(strip);

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
}

/******************************************************************************/
