/*******************************************************************************
 * random-sort-sound-pi/neopixel.cpp
 *
 * Copyright (C) 2015-2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#include <iostream>
#include <mutex>

#include <SDL.h>

#include <NeoAnimation/Animation/RandomAlgorithm.hpp>
#include <NeoAnimation/Strip/PiSPI_APA102.hpp>

/******************************************************************************/

#include <NeoAnimation/Animation/SortSound.hpp>

/******************************************************************************/

using namespace NeoAnimation;

PiSPI_APA102 my_strip(
    "/dev/spidev0.0", /* strip_size */ 5 * 96, /* cs_pin */ 24);

bool g_terminate = false;

int main() {
    srandom(time(nullptr));

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cout << "Couldn't initialize SDL: " << SDL_GetError() << std::endl;
        return 0;
    }

    // create audio output
    SDL_AudioSpec sdlaudiospec;

    // Set the audio format
    sdlaudiospec.freq = 44100;
    sdlaudiospec.format = AUDIO_S16SYS;
    sdlaudiospec.channels = 2;          // 1 = mono, 2 = stereo
    sdlaudiospec.samples = 1024;        // Good low-latency value for callback
    sdlaudiospec.callback = SoundCallback;
    sdlaudiospec.userdata = nullptr;

    // Open the audio device, forcing the desired format
    if (SDL_OpenAudio(&sdlaudiospec, nullptr) < 0) {
        std::cout << "Could not open audio: " << SDL_GetError() << std::endl;
        return 0;
    }

    SDL_PauseAudio(0);

    array_max = my_strip.size();

    // enable sound hook.
    NeoSort::SoundAccess = SoundAccess;

    RunRandomAlgorithmAnimation(my_strip);

    return 0;
}

/******************************************************************************/
