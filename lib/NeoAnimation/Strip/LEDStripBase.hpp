/*******************************************************************************
 * lib/NeoAnimation/Strip/LEDStripBase.hpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#ifndef NEOANIMATION_STRIP_LEDSTRIPBASE_HEADER
#define NEOANIMATION_STRIP_LEDSTRIPBASE_HEADER

namespace NeoAnimation {

/******************************************************************************/
// Base LED Strip

class LEDStripBase
{
public:
    uint8_t intensity() const {
        return intensity_;
    }

    void set_intensity(uint8_t intensity) {
        intensity_ = intensity;
    }

protected:
    uint8_t intensity_ = 255;
};

/******************************************************************************/
// Generic Remappers

template <typename BaseStrip, size_t Repeat>
class RepeatStrip : public BaseStrip
{
public:
    RepeatStrip(BaseStrip& base)
        : BaseStrip(base) { }

    size_t size() { return BaseStrip::size() / Repeat; }

    void setPixel(size_t i, const Color& c) {
        for (size_t r = 0; r < Repeat; ++r) {
            BaseStrip::setPixel(Repeat * i + r, c);
        }
    }

    void orPixel(size_t i, const Color& c) {
        for (size_t r = 0; r < Repeat; ++r) {
            BaseStrip::orPixel(Repeat * i + r, c);
        }
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
