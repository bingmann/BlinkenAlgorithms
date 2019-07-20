/*******************************************************************************
 * random-sort-sound-teensy/src/neopixel.cpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#include <Arduino.h>
#include <NeoAnimation/Porting/Teensy.hpp>

/******************************************************************************/

#include <NeoAnimation/Animation/RandomAlgorithm.hpp>
#include <NeoAnimation/Strip/OctoSK6812Adapter.hpp>

static const size_t strip_size = 300;

DMAMEM int displayMemory[strip_size * 8];
int drawMemory[strip_size * 8];

OctoSK6812 strip(strip_size, displayMemory, drawMemory, SK6812_GRBW);

using SK6812 = NeoAnimation::OctoSK6812Adapter<OctoSK6812>;
SK6812 my_strip(strip);

/******************************************************************************/

#include <NeoAnimation/Animation/SortSound.hpp>

#include <Audio.h>
#include <AudioStream.h>

class SortAudioStream : public AudioStream
{
public:
    SortAudioStream() : AudioStream(1, inputQueueArray) { }
    void update() final;

private:
    audio_block_t* inputQueueArray[1];
};

void SortAudioStream::update() {
    // allocate the audio blocks to transmit
    audio_block_t* block = allocate();
    if (block == nullptr) return;

    SoundCallback(
        nullptr, reinterpret_cast<uint8_t*>(block->data),
        AUDIO_BLOCK_SAMPLES * sizeof(int16_t));
    transmit(block);
    release(block);
}

SortAudioStream waveform1;
AudioOutputI2S i2s1;
AudioConnection patchCord1(waveform1, 0, i2s1, 0);

AudioControlSGTL5000 audioShield;

/******************************************************************************/

bool g_terminate = false;
size_t g_delay_factor = 1000;

void setup() {
    Serial.begin(115200);
    Serial.println("Starting...");

    randomSeed(analogRead(0));

    // the audio library needs to be given memory to start working
    AudioMemory(16);
    // enable the audio shield and set the output volume.
    audioShield.enable();
    audioShield.volume(1.0);

    strip.begin();

    // enable sorting sound output
    array_max = my_strip.size();
    // NeoSort::SoundAccessHook = SoundAccess;
}

using namespace NeoAnimation;

void loop() {
    RunRandomAlgorithmAnimation(my_strip);
}

/******************************************************************************/
