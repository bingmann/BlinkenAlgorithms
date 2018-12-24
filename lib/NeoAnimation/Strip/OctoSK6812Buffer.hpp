/*******************************************************************************
 * lib/NeoAnimation/Strip/OctoSK6812Buffer.hpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#ifndef NEOANIMATION_STRIP_OCTOSK6812BUFFER_HEADER
#define NEOANIMATION_STRIP_OCTOSK6812BUFFER_HEADER

#include <NeoAnimation/Strip/LEDStripBase.hpp>

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

namespace NeoAnimation {

template <typename OctoSK6812>
class OctoSK6812Buffer : public LEDStripBase
{
public:
    explicit OctoSK6812Buffer(OctoSK6812& strip, size_t active_parts = 8)
        : LEDStripBase(), strip_(strip),
          strip_size_(strip.numPixels() / 8), active_parts_(active_parts),
          buffer_(new Color[strip_size_ * active_parts]) {
        memset(buffer_, 0, sizeof(Color) * strip_size_ * active_parts_);
    }

    ~OctoSK6812Buffer() {
        delete[] buffer_;
    }

    size_t size() const {
        return strip_size_ * active_parts_;
    }

    void show() const {
        for (size_t i = 0; i < strip_size_ * active_parts_; ++i) {
            strip_.setPixel(i, buffer_[i].v);
        }
        return strip_.show();
    }

    bool busy() {
        return strip_.busy();
    }

    void setPixel(size_t i, const Color& c) {
        Color c2(gamma8[c.r], gamma8[c.g], gamma8[c.b], gamma8[c.w]);
        if (i < strip_size_ * active_parts_) {
            buffer_[i] = c2;
        }
    }

    void orPixel(size_t i, const Color& c) {
        Color c2(gamma8[c.r], gamma8[c.g], gamma8[c.b], gamma8[c.w]);
        if (i < strip_size_ * active_parts_) {
            Color c1 = buffer_[i];
            buffer_[i] = Color(
                c1.r | c2.r, c1.g | c2.g, c1.b | c2.b, c1.w | c2.w);
        }
    }

private:
    OctoSK6812& strip_;

    size_t strip_size_;
    size_t active_parts_;

    Color* buffer_;

    std::vector<uint8_t> gamma8 = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
        2, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5,
        5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10,
        10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
        17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
        25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
        37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
        51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
        69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
        90, 92, 93, 95, 96, 98, 99, 101, 102, 104, 105, 107, 109, 110, 112, 114,
        115, 117, 119, 120, 122, 124, 126, 127, 129, 131, 133, 135, 137, 138, 140, 142,
        144, 146, 148, 150, 152, 154, 156, 158, 160, 162, 164, 167, 169, 171, 173, 175,
        177, 180, 182, 184, 186, 189, 191, 193, 196, 198, 200, 203, 205, 208, 210, 213,
        215, 218, 220, 223, 225, 228, 231, 233, 236, 239, 241, 244, 247, 249, 252, 255
    };
};

} // namespace NeoAnimation

#endif // !NEOANIMATION_STRIP_OCTOSK6812BUFFER_HEADER

/******************************************************************************/
