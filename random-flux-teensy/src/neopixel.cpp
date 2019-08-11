/*******************************************************************************
 * random-flux-teensy/src/neopixel.cpp
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

OctoSK6812 base_strip(strip_size, displayMemory, drawMemory, SK6812_GRBW);

using Strip = OctoSK6812Adapter<OctoSK6812>;
Strip strip(base_strip, /* active_parts */ 8);

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

    base_strip.begin();
}

void loop() {
    RunRandomFluxAnimations(strip);
}

/******************************************************************************/
