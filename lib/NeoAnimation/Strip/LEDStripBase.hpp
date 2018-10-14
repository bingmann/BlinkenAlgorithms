/*******************************************************************************
 * lib/NeoAnimation/Strip/LEDStripBase.hpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#ifndef NEOANIMATION_STRIP_LEDSTRIPBASE_HEADER
#define NEOANIMATION_STRIP_LEDSTRIPBASE_HEADER

#if ESP8266
// no includes
#else
#include <chrono>
#include <thread>
#endif

namespace NeoAnimation {

/******************************************************************************/
// Base LED Strip

class LEDStripBase
{
public:
    uint8_t intensity() {
        return 255;
    }

    unsigned long millis() {
#if ESP8266
        return ::millis();
#else
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
#endif
    }

    unsigned long micros() {
#if ESP8266
        return ::micros();
#else
        return std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
#endif
    }

    void delay(uint32_t msec) {
        delay_micros(msec * 1000);
    }

    void delay_micros(uint32_t usec) {
#if ESP8266
        ESP.wdtFeed();
        if (usec != 0)
            delayMicroseconds(usec);
#else
        if (usec != 0)
            std::this_thread::sleep_for(std::chrono::microseconds(usec));
#endif
    }
};

/******************************************************************************/
// Milliways Dome @ EMF 2018

template <typename BaseStrip>
class LEDMultiDomeStrip : public BaseStrip
{
public:
    LEDMultiDomeStrip(BaseStrip& base)
        : BaseStrip(base) { }

    size_t size() { return 300; }

    void setPixel(size_t i, const Color& c) {
        for (size_t s = 0; s < 5; ++s) {
            BaseStrip::setPixel(s * 300 + i, c);
        }
    }
};

template <typename BaseStrip>
class LEDDomeFFTStrip : public BaseStrip
{
public:
    LEDDomeFFTStrip(BaseStrip& base)
        : BaseStrip(base) { }

    size_t size() { return 260; }

    void setPixel(size_t i, const Color& c) {
        for (size_t s = 0; s < 5; ++s) {
            BaseStrip::setPixel(s * 300 + 260 - 1 - i, c);
        }
    }
};

template <typename BaseStrip>
class LEDDomeBarStrip : public BaseStrip
{
public:
    LEDDomeBarStrip(BaseStrip& base)
        : BaseStrip(base) { }

    size_t size() { return 600; }

    void setPixel(size_t i, const Color& c) {
        if (i < 300)
            BaseStrip::setPixel(i + 5 * 300, c);
        else if (i < 600)
            BaseStrip::setPixel(7 * 300 - (i - 300) - 1, c);
    }
};

} // namespace NeoAnimation

#endif // !NEOANIMATION_STRIP_LEDSTRIPBASE_HEADER

/******************************************************************************/
