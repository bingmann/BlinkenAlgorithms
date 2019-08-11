/*******************************************************************************
 * lib/BlinkenAlgorithms/BlinkenAlgorithms/Strip/AdafruitNeoPixelAdapter.hpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#ifndef BLINKENALGORITHMS_STRIP_ADAFRUITNEOPIXELADAPTER_HEADER
#define BLINKENALGORITHMS_STRIP_ADAFRUITNEOPIXELADAPTER_HEADER

#include <BlinkenAlgorithms/Strip/LEDStripBase.hpp>

#include <Adafruit_NeoPixel.h>

namespace BlinkenAlgorithms {

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

    Color getPixel(size_t i) const {
        return ColorRGBW(strip_.getPixelColor(i));
    }

private:
    Adafruit_NeoPixel& strip_;
};

} // namespace BlinkenAlgorithms

#endif // !BLINKENALGORITHMS_STRIP_ADAFRUITNEOPIXELADAPTER_HEADER

/******************************************************************************/
