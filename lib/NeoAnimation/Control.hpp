/*******************************************************************************
 * lib/NeoAnimation/Control.hpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#ifndef NEOANIMATION_CONTROL_HEADER
#define NEOANIMATION_CONTROL_HEADER

#include <cstdlib>

#if ESP8266
// no includes
#else
#include <chrono>
#include <thread>
#endif

extern bool g_terminate;
extern size_t g_delay_factor;

/******************************************************************************/
// Raspberry Pi: add missing global functions

#if !ESP8266 && !TEENSYDUINO
static inline
uint32_t random(uint32_t limit) {
    return ::random() % limit;
}

static inline
uint32_t random(uint32_t begin, uint32_t limit) {
    return ::random() % (limit - begin) + begin;
}
#endif

#if !ESP8266 && !TEENSYDUINO
static inline
unsigned long millis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
}
#endif

/******************************************************************************/
// Timing, Delay, and Control Methods

namespace NeoAnimation {

static inline
unsigned long micros() {
#if ESP8266 || TEENSYDUINO
    return ::micros();
#else
    return std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
#endif
}

static inline
void delay_micros(uint32_t usec) {
#if ESP8266
    ESP.wdtFeed();
    if (usec != 0)
        delayMicroseconds(usec);
#elif TEENSYDUINO
    if (usec != 0)
        delayMicroseconds(usec);
#else
    if (usec != 0)
        std::this_thread::sleep_for(std::chrono::microseconds(usec));
#endif
}

static inline
void delay_millis(uint32_t msec) {
    delay_micros(msec * 1000);
}

} // namespace NeoAnimation

/******************************************************************************/
// Debug printing

#if TEENSYDUINO

namespace NeoAnimation {

// Serial.printf("Running Algorithm %d\n", a);

} // namespace NeoAnimation

#endif // TEENSYDUINO

#endif // !NEOANIMATION_CONTROL_HEADER

/******************************************************************************/
