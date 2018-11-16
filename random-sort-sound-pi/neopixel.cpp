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
#include <linux/input.h>

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
size_t g_delay_factor = 1000;

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

int fd_kbd = -1;

static const char *const ev_value_text[3] = {
    "RELEASED","PRESSED ", "REPEATED"
};

enum EventValue { RELEASED = 0, PRESSED = 1, REPEATED = 2 };

enum class Mode {
    Random,
    RandomSort,
    RandomHash,
    Black,
    InsertionSort,
    QuickSortFast,
    QuickSortSlow,
    MergeSort,
};

Mode g_mode = Mode::Random;

void SwitchTo(Mode mode) {
    g_mode = mode;
    g_delay_factor = 1000;
    g_terminate = true;
    throw std::runtime_error("restart");
 }

void OnDelay() {
    led_matrix.clear();
    mfont.print(s_algo_name, led_matrix);
    mfont.print_right(s_algo_stats, led_matrix);
    led_matrix.show();

    struct input_event ev;
    ssize_t n = read(fd_kbd, &ev, sizeof(ev));
    if (n == sizeof(ev)) {
        if (ev.type != EV_KEY)
            return;

    PROCESS:
        if (ev.value >= RELEASED && ev.value <= REPEATED) {
            printf("%s 0x%04x (%d)\n", ev_value_text[ev.value],
                   (int)ev.code, (int)ev.code);
        }

        if (ev.code == 55 && ev.value == RELEASED) { // ESC released
            SwitchTo(Mode::Random);
        }
        else if ((ev.code == 82 || ev.code == 110) && ev.value == RELEASED) {
            // 0 released
            SwitchTo(Mode::Black);
        }
        else if ((ev.code == 79 || ev.code == 107) && ev.value == RELEASED) {
            // 1 released
            SwitchTo(Mode::InsertionSort);
        }
        else if ((ev.code == 80 || ev.code == 108) && ev.value == RELEASED) {
            // 2/@ released
            SwitchTo(Mode::QuickSortFast);
        }
        else if ((ev.code == 81 || ev.code == 109) && ev.value == RELEASED) {
            // 3/# released
            SwitchTo(Mode::QuickSortSlow);
        }
        else if ((ev.code == 75 || ev.code == 105) && ev.value == RELEASED) {
            // 4/$ released
            SwitchTo(Mode::MergeSort);
        }
        else if (ev.code == 96 && ev.value == RELEASED) { // SPACE released
            // pause
            while (true) {
                n = read(fd_kbd, &ev, sizeof(ev));
                if (n != sizeof(ev) || ev.type != EV_KEY) {
                    delay_micros(1000);
                    continue;
                }

                if (ev.code == 96 && ev.value == PRESSED) {
                    continue;
                }
                else if (ev.code == 96 && ev.value == RELEASED) {
                    return;
                }
                else {
                    goto PROCESS;
                }
            }
        }
        else if (ev.code == 14 && ev.value == RELEASED) { // ENTER released
            g_terminate = true;
            throw std::runtime_error("restart");
        }
        else if (ev.code == 78 &&
                 (ev.value == PRESSED || ev.value == REPEATED)) {
            // UP pressed
            g_delay_factor = g_delay_factor * 1000 / 1100;
            if (g_delay_factor < 10)
                g_delay_factor = 10;
            std::cout << "g_delay_factor " << g_delay_factor << std::endl;
        }
        else if (ev.code == 74 &&
                 (ev.value == PRESSED || ev.value == REPEATED)) {
            // DOWN pressed
            g_delay_factor = g_delay_factor * 1000 / 900;
            if (g_delay_factor > 100000)
                g_delay_factor = 100000;
            std::cout << "g_delay_factor " << g_delay_factor << std::endl;
        }
    }
}

void wait_millis(uint32_t msec) {
    uint32_t remain = msec;
    while (remain >= 100) {
        delay_millis(100);
        remain -= 100;
        OnDelay();
    }
    delay_millis(remain);
    OnDelay();
}

void wait_forever() {
    while (true) {
        delay_micros(1000);
        OnDelay();
    }
}

int main() {
    srandom(time(nullptr));

    // ---[ Open USB Keyboard ]-------------------------------------------------

    const char *dev = "/dev/input/by-id/usb-_USB_Keyboard-event-kbd";

    fd_kbd = open(dev, O_RDONLY);
    if (fd_kbd < 0)
        fprintf(stderr, "Cannot open keyboard %s: %s.\n", dev, strerror(errno));

    int flags = fcntl(fd_kbd, F_GETFL, 0);
    fcntl(fd_kbd, F_SETFL, flags | O_NONBLOCK);

    // ---[ Initialize Audio ]--------------------------------------------------

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

    // ---[ Run Animation ]-----------------------------------------------------

    array_max = my_strip.size();

    // enable hooks
    NeoSort::SoundAccessHook = OnSoundAccess;
    NeoSort::DelayHook = OnDelay;
    NeoSort::ComparisonCountHook = OnComparisonCount;
    NeoSort::AlgorithmNameHook = OnAlgorithmName;

    using namespace NeoSort;

    while (1) {
        try {
            g_terminate = false;

            switch (g_mode) {
            case Mode::Random:
                RunRandomAlgorithmAnimation(my_strip);
                wait_millis(3000);
                break;

            case Mode::Black:
                for (size_t i = 0; i < my_strip.size(); ++i) {
                    my_strip.setPixel(i, 0);
                }
                my_strip.show();
                wait_forever();
                break;

            case Mode::InsertionSort:
                RunSort(my_strip, InsertionSort, -21);
                wait_forever();
                break;
            case Mode::QuickSortFast:
                RunSort(my_strip, QuickSortLR, -21);
                wait_forever();
                break;
            case Mode::QuickSortSlow:
                RunSort(my_strip, QuickSortLR, 10);
                wait_forever();
                break;
            case Mode::MergeSort:
                RunSort(my_strip, MergeSort, 10);
                wait_forever();
                break;
            }
        }
        catch (std::exception& e) {
            std::cout << "EXCEPTION: " << e.what() << std::endl;
        }
    }

    return 0;
}

/******************************************************************************/
