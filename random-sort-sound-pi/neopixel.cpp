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

//! all time counters in the sound system are in sample units.
static const size_t s_samplerate = 44100;

//! global sound on/off switch
bool g_sound_on = true;

//! multiplying g_delay with this value yields the duration a sound is sustained
double g_sound_sustain = 2.0;

//! limit the number of oscillators to avoid overloading the callback
static const size_t s_max_oscillators = 348;

//! Oscillator generating sine or triangle waves
class Oscillator
{
protected:
    //! frequency of generated wave
    float m_freq;

    //! start and end of wave in sample time
    size_t m_tstart, m_tend;

    //! duration of oscillation note
    size_t m_duration;

public:
    //! construct new oscillator
    Oscillator(float freq, size_t tstart, size_t duration = 44100 / 8)
        : m_freq(freq), m_tstart(tstart),
          m_tend(m_tstart + duration),
          m_duration(duration)
    { }

    // *** Wave Forms

    //! triangle wave
    static int32_t wave_sin(int32_t x) {
        x &= 0xFFFF;
        return sin(static_cast<float>(x) / 0xFFFF) * 0xFFFF;
    }

    //! triangle wave
    static int32_t wave_triangle(int32_t x) {
        x &= 0xFFFF;

        if (x <= 65536 / 4)
            return x;
        if (x <= 3 * 65536 / 4)
            return 65536 / 2 - x;
        return x - 65536;
    }

    //! picking a waveform
    static int32_t wave(int32_t x) {
        // return wave_sin(x);
        return wave_triangle(x);
    }

    // *** Envelope

    //! envelope applied to wave (uses ADSR)
    int32_t envelope(size_t i) const {
        static const uint32_t unit = 65536;
        uint32_t x = i / m_duration;
        if (x > unit) x = unit;

        // simple envelope functions:

        // return 1.0 - x;
        // return cos(M_PI_2 * x);

        // *** ADSR envelope

        static const uint32_t attack = 0.2 * unit;   // percentage of duration
        static const uint32_t decay = 0.2 * unit;    // percentage of duration
        static const uint32_t sustain = 0.4 * unit;  // percentage of amplitude
        static const uint32_t release = 0.2 * unit;  // percentage of duration

        if (x < attack)
            return unit / attack * x;

        if (x < attack + decay)
            return unit - (x - attack) / decay * (unit - sustain);

        if (x < unit - release)
            return sustain;

        return sustain / release * (unit - x);
    }

    // *** Generate Wave and Mix

    int32_t mix_value(size_t p) const {
        if (p < m_tstart)
            return 0;
        if (p >= m_tend)
            return 0;

        size_t trel = (p - m_tstart);

        return wave(trel * 65536 * m_freq / s_samplerate)
               * envelope(trel * 65556);
    }

    //! return start time
    size_t tstart() const {
        return m_tstart;
    }

    //! true if the oscillator is silent at time p
    bool is_done(size_t p) const {
        return (p >= m_tend);
    }
};

//! array of oscillators
static std::vector<Oscillator> s_osclist;

//! global timestamp of callback in sound sample units
static size_t s_pos = 0;

//! add an oscillator to the list (reuse finished ones)
static void add_oscillator(float freq, size_t p, size_t pstart, size_t duration) {
    // find free oscillator
    size_t oldest = 0, toldest = std::numeric_limits<size_t>::max();
    for (size_t i = 0; i < s_osclist.size(); ++i)
    {
        if (s_osclist[i].is_done(p)) {
            s_osclist[i] = Oscillator(freq, pstart, duration);
            return;
        }

        if (s_osclist[i].tstart() < toldest) {
            oldest = i;
            toldest = s_osclist[i].tstart();
        }
    }

    if (s_osclist.size() < s_max_oscillators)
    {
        // add new one
        s_osclist.push_back(Oscillator(freq, pstart, duration));
    }
    else
    {
        // replace oldest oscillator
        s_osclist[oldest] = Oscillator(freq, pstart, duration);
    }
}

//! list of array accesses since last callback
static std::vector<unsigned> s_access_list;

// mutex to s_access_list
static std::mutex s_mutex_access_list;

//! "public" function to add a new array access
static void SoundAccess(size_t i) {
    if (!g_sound_on) return;

    std::unique_lock<std::mutex> lock(s_mutex_access_list);
    s_access_list.push_back(i);
}

//! function mapping array index (normalized to [0,1]) to frequency
static float arrayindex_to_frequency(float aindex) {
    return 120 + 1200 * (aindex * aindex);
}

//! reset internal sound data (called from main thread)
void SoundReset() {
    std::unique_lock<std::mutex> lock(s_mutex_access_list);

    s_pos = 0;
    s_osclist.clear();
}

static size_t array_max = 0;
static float g_delay = 25;

//! sound generator callback run by SDL
void SoundCallback(void* /* udata */, Uint8* stream, int len) {
    if (!g_sound_on) {
        memset(stream, 0, len);
        return;
    }

    // current sample time (32-bit size_t wraps after 27 hours, 64-bit size_t
    // wraps after 13 million years).
    size_t& p = s_pos;

    // we use 16-bit mono output at 44.1 kHz
    int16_t* data = (int16_t*)stream;
    size_t size = len / sizeof(int16_t);

    // fetch new access list and create oscillators
    {
        std::unique_lock<std::mutex> lock(s_mutex_access_list);

        // spread out access list over time of one callback
        float pscale = (float)size / s_access_list.size();

        for (size_t i = 0; i < s_access_list.size(); ++i)
        {
            float freq = arrayindex_to_frequency(
                s_access_list[i] / static_cast<float>(array_max));

            add_oscillator(freq, p, p + i * pscale,
                           g_delay / 1000.0 * g_sound_sustain * s_samplerate);
        }

        s_access_list.clear();
    }

    static int32_t volume_factor = 100;
    int32_t this_maximum = 0.0;

    for (size_t i = 0; i < size; ++i) {
        int32_t v = 0;

        for (std::vector<Oscillator>::const_iterator it = s_osclist.begin();
             it != s_osclist.end(); ++it)
        {
            if (!it->is_done(p))
                v += it->mix_value(p + i) >> 16;
        }

        v = (v * volume_factor) >> 16;

        if (v > 32760)
            v = 32760;
        if (v < -32760)
            v = -32760;

        this_maximum = std::max(this_maximum, std::abs(v));

        data[i] = v;
    }

    // std::cout << "this_maximum " << this_maximum
    //           << " this_maximum " << this_maximum
    //           << " volume_factor " << volume_factor
    //           << std::endl;

    if (this_maximum < 20000) {
        volume_factor = volume_factor * 120 / 100;
    }
    else if (this_maximum < 24000) {
        volume_factor = volume_factor * 105 / 100;
    }
    else if (this_maximum > 48000) {
        volume_factor = volume_factor * 50 / 100;
    }
    else if (this_maximum > 36000) {
        volume_factor = volume_factor * 90 / 100;
    }
    else if (this_maximum > 24000) {
        volume_factor = volume_factor * 95 / 100;
    }

    // advance sample timestamp
    p += size;
}

/******************************************************************************/

using namespace NeoAnimation;

PiSPI_APA102 my_strip("/dev/spidev0.0", /* strip_size */ 5 * 96);

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
    sdlaudiospec.channels = 1;          // 1 = mono, 2 = stereo
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
