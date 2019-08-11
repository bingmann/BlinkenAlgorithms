/*******************************************************************************
 * random-flux-pi/neopixel.cpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#include <BlinkenAlgorithms/Porting/RaspberryPi.hpp>

#include <BlinkenAlgorithms/Animation/Flux.hpp>
#include <BlinkenAlgorithms/RunAnimation.hpp>
#include <BlinkenAlgorithms/Strip/PiSPI_APA102.hpp>

using namespace BlinkenAlgorithms;

using Strip = PiSPI_APA102;
Strip strip("/dev/spidev0.0", /* strip_size */ 5 * 96);

bool g_terminate = false;

void delay_poll() { }

int main() {
    srandom(time(nullptr));

    RunRandomFluxAnimations(strip);

    return 0;
}

/******************************************************************************/
