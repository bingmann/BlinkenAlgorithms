/*******************************************************************************
 * lib/NeoAnimation/Strip/AdafruitNeoPixelAdapter.hpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#ifndef NEOANIMATION_STRIP_ADAFRUITNEOPIXELADAPTER_HEADER
#define NEOANIMATION_STRIP_ADAFRUITNEOPIXELADAPTER_HEADER

#include <NeoAnimation/Strip/LEDStripBase.hpp>

#include <Adafruit_NeoPixel.h>

namespace NeoAnimation {

class AdafruitNeoPixelAdapter : public LEDStripBase
{
public:
    explicit AdafruitNeoPixelAdapter(Adafruit_NeoPixel& strip)
        : LEDStripBase(), strip_(strip) { }

    size_t size() const {
        return strip_.numPixels();
    }

    void show() const {
        return strip_.show();
    }

    bool busy() {
        return !strip_.canShow();
    }

    void setPixel(size_t i, const Color& c) {
        strip_.setPixelColor(i, gamma8(c.r), gamma8(c.g),
                             gamma8(c.b), gamma8(c.w));
    }

    void orPixel(size_t i, const Color& c) {
        uint32_t p = strip_.getPixelColor(i);
        uint8_t w = (p >> 24) & 0xFF;
        uint8_t r = (p >> 16) & 0xFF;
        uint8_t g = (p >> 8) & 0xFF;
        uint8_t b = (p >> 0) & 0xFF;
        setPixel(i, Color(gamma8(c.r) | r, gamma8(c.g) | g,
                          gamma8(c.b) | b, gamma8(c.w) | w));
    }

private:
    Adafruit_NeoPixel& strip_;
};

} // namespace NeoAnimation

#endif // !NEOANIMATION_STRIP_ADAFRUITNEOPIXELADAPTER_HEADER

/******************************************************************************/
