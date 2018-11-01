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

#include <NeoAnimation/Extra/Font5x5.hpp>
#include <NeoAnimation/Extra/MAX7219.hpp>

/******************************************************************************/

#include <NeoAnimation/Animation/SortSound.hpp>

/******************************************************************************/

using namespace NeoAnimation;

PiSPI_APA102 my_strip(
    "/dev/spidev0.0", /* strip_size */ 5 * 96, /* cs_pin */ 24);

bool g_terminate = false;

MAX7219 led_matrix("/dev/spidev0.0", /* cs_pin */ 23);
Font5x5 mfont;

const char* s_algo_name = "";
char s_algo_stats[64];

void OnComparisonCount(size_t count) {
    snprintf(s_algo_stats, sizeof(s_algo_stats), "%zu", count);
}

void OnAlgorithmName(const char* name) {
    s_algo_name = name;
    OnComparisonCount(0);
}

void OnDelay() {
    led_matrix.clear();
    mfont.print(s_algo_name, led_matrix);
    mfont.print_right(s_algo_stats, led_matrix);
    led_matrix.show();
}

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

    // enable hooks
    NeoSort::SoundAccessHook = OnSoundAccess;
    NeoSort::DelayHook = OnDelay;
    NeoSort::ComparisonCountHook = OnComparisonCount;
    NeoSort::AlgorithmNameHook = OnAlgorithmName;

    RunRandomAlgorithmAnimation(my_strip);

    return 0;
}

/******************************************************************************/
