/*******************************************************************************
 * lib/BlinkenAlgorithms/BlinkenAlgorithms/Strip/OctoSK6812Adapter.hpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#ifndef BLINKENALGORITHMS_STRIP_OCTOSK6812ADAPTER_HEADER
#define BLINKENALGORITHMS_STRIP_OCTOSK6812ADAPTER_HEADER

#include <BlinkenAlgorithms/Strip/LEDStripBase.hpp>

/******************************************************************************/

/*
  Required Connections
  --------------------
    pin 2:  LED Strip #1    OctoWS2811 drives 8 LED Strips.
    pin 14: LED strip #2    All 8 are the same length.
    pin 7:  LED strip #3
    pin 8:  LED strip #4    A 100 ohm resistor should used
    pin 6:  LED strip #5    between each Teensy pin and the
    pin 20: LED strip #6    wire to the LED strip, to minimize
    pin 21: LED strip #7    high frequency ringining & noise.
    pin 5:  LED strip #8
    pin 15 & 16 - Connect together, but do not use
    pin 4 - Do not use
    pin 3 - Do not use as PWM.  Normal use is ok.
*/

#include <OctoSK6812.h>

namespace BlinkenAlgorithms {

template <typename OctoSK6812>
class OctoSK6812Adapter : public LEDStripBase
{
public:
    explicit OctoSK6812Adapter(OctoSK6812& strip, size_t active_parts = 8)
        : LEDStripBase(), strip_(strip), active_parts_(active_parts) { }

    size_t size() const {
        return strip_.numPixels() * active_parts_ / 8;
    }

    void show() const {
        return strip_.show();
    }

    bool busy() {
        return strip_.busy();
    }

    void setPixel(size_t i, const Color& c) {
        Color c2(gamma8(c.r), gamma8(c.g), gamma8(c.b), gamma8(c.w));
        setPixelRaw(i, c2);
    }

    Color getPixel(size_t i) const {
        return ColorRGBW(strip_.getPixel(i));
    }

    void orPixel(size_t i, const Color& c) {
        Color c1 = strip_.getPixel(i);
        Color c2(gamma8(c.r), gamma8(c.g), gamma8(c.b), gamma8(c.w));
        setPixelRaw(i, c1 | c2);
    }

    void addPixel(size_t i, const Color& c) {
        Color c1 = strip_.getPixel(i);
        Color c2(gamma8(c.r), gamma8(c.g), gamma8(c.b), gamma8(c.w));
        setPixelRaw(i, c1 + c2);
    }

private:
    OctoSK6812& strip_;

    size_t active_parts_;

    void setPixelRaw(size_t i, const Color& c) {
        strip_.setPixel(i, c.v);
    }
};

} // namespace BlinkenAlgorithms

#endif // !BLINKENALGORITHMS_STRIP_OCTOSK6812ADAPTER_HEADER

/******************************************************************************/
