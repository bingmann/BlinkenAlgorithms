/*******************************************************************************
 * blinken-sort-sound-pi/blinken-sort.cpp
 *
 * Copyright (C) 2015-2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#include <iostream>
#include <mutex>

#include <linux/input.h>
#include <SDL.h>

#include <BlinkenAlgorithms/Animation/Hashtable.hpp>
#include <BlinkenAlgorithms/Animation/LawaSAT.hpp>
#include <BlinkenAlgorithms/Animation/Sort.hpp>

#include <BlinkenAlgorithms/Strip/PiSPI_APA102.hpp>

#include <BlinkenAlgorithms/Extra/Font5x5.hpp>
#include <BlinkenAlgorithms/Extra/MAX7219.hpp>

#include <BlinkenAlgorithms/Animation/SortSound.hpp>

/******************************************************************************/

/*!
 * BlinkenAlgorithms with delay time adapted for ESP8266 and Teensy 3.6 such
 * that most algorithms run approximately 15 seconds each for a strip with 300
 * LEDs.  Asymptotically slower algorithms run 20 seconds, faster ones 10 or
 * less.
 */
template <typename LEDStrip>
void RunRandomAlgorithm(LEDStrip& strip) {
    for (size_t i = 0; i < strip.size(); ++i)
        strip.setPixel(i, 0);

    using namespace BlinkenSort;
    using namespace BlinkenHashtable;
    using namespace BlinkenLawaSAT;

    size_t a = random(22);
    switch (a) {
    case 0:
        RunSort(strip, "Selection Sort", SelectionSort, -44); // 60 secs
        break;
    case 1:
        RunSort(strip, "Insertion Sort", InsertionSort, -40); // 50 secs
        break;
    case 2:
        RunSort(strip, "Bubble Sort", BubbleSort, -60); // 63 secs
        break;
    case 3:
        RunSort(strip, "Cocktail-Shaker Sort", CocktailShakerSort, -50); // 58 secs
        break;
    case 4:
        RunSort(strip, "QuickSort (LR)\nHoare", QuickSortLR, -6); // 27 secs
        break;
    case 5:
        RunSort(strip, "QuickSort (LL)\nLomoto", QuickSortLL, -6); // 27 secs
        break;
    case 6:
        RunSort(strip, "QuickSort\nDual Pivot", QuickSortDualPivot, -4); // 24 secs
        break;
    case 7:
        RunSort(strip, "MergeSort", MergeSort, -2); // 24 secs
        break;
    case 8:
        RunSort(strip, "ShellSort", ShellSort, -3); // 42 secs
        break;
    case 9:
        RunSort(strip, "HeapSort", HeapSort, -6); // 42 secs
        break;
    case 10:
        RunSort(strip, "CycleSort", CycleSort, 16000); // 40 secs
        break;
    case 11:
        RunSort(strip, "RadixSort-MSD\n(High First)", RadixSortMSD, -4); // 27 secs
        break;
    case 12:
        RunSort(strip, "RadixSort-LSD\n(Low First)", RadixSortLSD, -1); // 27 secs
        break;
    case 13:
        RunSort(strip, "std::sort", StdSort, -6); // 22 secs
        break;
    case 14:
        RunSort(strip, "std::stable_sort", StdStableSort, -3); // 28 secs
        break;
    case 15:
        RunSort(strip, "WikiSort", WikiSort, -6); // 42 secs
        break;
    case 16:
        RunSort(strip, "TimSort", TimSort, -4); // 27 secs
        break;
    case 17:
        RunSort(strip, "BozoSort", BozoSort, -4); // 20 secs (break time)
        break;

    /*------------------------------------------------------------------------*/

    case 18:
        RunHash(strip, "Linear Probe\nHash Table", // 40.8 secs
                LinearProbingHT, 0);
        break;
    case 19:
        RunHash(strip, "Quadratic Probe Hash Table", // 35 secs
                QuadraticProbingHT, 4000);
        break;
    case 20:
        RunHash(strip, "Cuckoo Two\nHash Table", // 35 secs
                CuckooHashingTwo, 12000);
        break;
    case 21:
        RunHash(strip, "Cuckoo Three\nHash Table",  // 35 secs
                CuckooHashingThree, 6000);
        break;

    /*------------------------------------------------------------------------*/

        // case 22:
        //     RunLawaSAT(strip);
        //     break;
    }
}

/******************************************************************************/

using namespace BlinkenAlgorithms;

PiSPI_APA102 my_strip("/dev/spidev0.0", /* strip_size */ 5 * 96);

bool g_terminate = false;
size_t g_delay_factor = 1000;

MAX7219 led_matrix("/dev/spidev1.0", /* cs_pin */ 23);
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

static const char* const ev_value_text[3] = {
    "RELEASED", "PRESSED ", "REPEATED"
};

enum EventValue { RELEASED = 0, PRESSED = 1, REPEATED = 2 };

enum class Mode {
    Random,
    RandomSort,
    RandomHash,
    Blank,
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
            SwitchTo(Mode::Blank);
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

    const char* dev = "/dev/input/by-id/usb-_USB_Keyboard-event-kbd";

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

    // set intensity
    my_strip.set_intensity(128);
    BlinkenSort::intensity_flash_high = 140;

    // enable hooks
    BlinkenSort::SoundAccessHook = OnSoundAccess;
    BlinkenSort::DelayHook = OnDelay;
    BlinkenSort::ComparisonCountHook = OnComparisonCount;
    BlinkenSort::AlgorithmNameHook = OnAlgorithmName;

    using namespace BlinkenSort;

    while (1) {
        try {
            g_terminate = false;

            switch (g_mode) {
            case Mode::Random:
                RunRandomAlgorithm(my_strip);
                break;

            case Mode::Blank:
                s_algo_name = "";
                OnComparisonCount(0);
                for (size_t i = 0; i < my_strip.size(); ++i) {
                    my_strip.setPixel(i, 0);
                }
                my_strip.show();
                wait_forever();
                break;

            case Mode::InsertionSort:
                RunSort(my_strip, "Insertion Sort", InsertionSort, -21);
                wait_forever();
                break;
            case Mode::QuickSortFast:
                RunSort(my_strip, "QuickSort LR", QuickSortLR, -21);
                wait_forever();
                break;
            case Mode::QuickSortSlow:
                RunSort(my_strip, "QuickSort LR", QuickSortLR, 10);
                wait_forever();
                break;
            case Mode::MergeSort:
                RunSort(my_strip, "Merge Sort", MergeSort, 10);
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
