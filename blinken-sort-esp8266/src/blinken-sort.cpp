/*******************************************************************************
 * blinken-sort-esp8266/src/blinken-sort.cpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#include <Arduino.h>

#include <BlinkenAlgorithms/Animation/RandomAlgorithm.hpp>
#include <BlinkenAlgorithms/Strip/NeoPixelBusAdapter.hpp>

#include <LiquidCrystal_I2C.h>

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
size_t g_delay_factor = 1000;

// set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

void OnAlgorithmName(const char* name) {
    lcd.clear();
    size_t x = 0, y = 0;
    lcd.setCursor(0, 0);
    for (size_t i = 0; name[i]; ++i) {
        if (name[i] == '\n') {
            lcd.setCursor(x = 0, ++y);
        }
        else {
            lcd.print(name[i]);
            if (++x == 16) {
                lcd.setCursor(x = 0, ++y);
            }
        }
    }
}

void setup() {
    Serial.begin(115200);

    randomSeed(analogRead(0));
    strip.Begin();

    // set the LCD address to 0x27 for a 16 chars and 2 line display
    lcd.init();

    // print a message to the LCD.
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("BlinkenSort");

    // add hook to print algorithm name
    BlinkenSort::AlgorithmNameHook = OnAlgorithmName;

    BlinkenSort::intensity_flash_high = 255;
}

using namespace BlinkenAlgorithms;

void loop() {
    NeoPixelBusAdapter<decltype(strip)> my_strip(strip);
    RunRandomAlgorithmAnimation(my_strip);
}

/******************************************************************************/
