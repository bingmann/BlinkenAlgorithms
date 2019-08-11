/*******************************************************************************
 * lib/BlinkenAlgorithms/BlinkenAlgorithms/Strip/OctoSK6812Buffer.hpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#ifndef BLINKENALGORITHMS_STRIP_OCTOSK6812BUFFER_HEADER
#define BLINKENALGORITHMS_STRIP_OCTOSK6812BUFFER_HEADER

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
class OctoSK6812Buffer : public LEDStripBase
{
public:
    OctoSK6812Buffer(OctoSK6812& strip, size_t active_parts,
                     size_t active_size)
        : LEDStripBase(), strip_(strip),
          strip_size_(strip.numPixels() / 8),
          active_parts_(active_parts),
          active_size_(active_size),
          buffer_(new Color[active_size]) {
        memset(buffer_, 0, sizeof(Color) * active_size_);
    }

    explicit OctoSK6812Buffer(OctoSK6812& strip, size_t active_parts = 8)
        : OctoSK6812Buffer(strip, active_parts,
                           strip.numPixels() / 8 * active_parts) { }

    ~OctoSK6812Buffer() {
        delete[] buffer_;
    }

    size_t size() const {
        return active_size_;
    }

    void show() const {
        for (size_t i = 0; i < active_size_; ++i) {
            strip_.setPixel(i, buffer_[i].v);
        }
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
        return i < active_size_ ? buffer_[i] : Color(0);
    }

    void orPixel(size_t i, const Color& c) {
        Color c1 = getPixel(i);
        Color c2(gamma8(c.r), gamma8(c.g), gamma8(c.b), gamma8(c.w));
        setPixelRaw(i, c1 | c2);
    }

    void addPixel(size_t i, const Color& c) {
        Color c1 = getPixel(i);
        Color c2(gamma8(c.r), gamma8(c.g), gamma8(c.b), gamma8(c.w));
        setPixelRaw(i, c1 + c2);
    }

private:
    OctoSK6812& strip_;

    size_t strip_size_;
    size_t active_parts_;
    size_t active_size_;

    Color* buffer_;

    void setPixelRaw(size_t i, const Color& c) {
        if (i < active_size_)
            buffer_[i] = c;
    }
};

} // namespace BlinkenAlgorithms

#endif // !BLINKENALGORITHMS_STRIP_OCTOSK6812BUFFER_HEADER

/******************************************************************************/
