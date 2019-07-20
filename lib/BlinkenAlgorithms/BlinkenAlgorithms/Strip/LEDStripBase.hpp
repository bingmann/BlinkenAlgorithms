/*******************************************************************************
 * lib/BlinkenAlgorithms/BlinkenAlgorithms/Strip/LEDStripBase.hpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#ifndef BLINKENALGORITHMS_STRIP_LEDSTRIPBASE_HEADER
#define BLINKENALGORITHMS_STRIP_LEDSTRIPBASE_HEADER

#include <cassert>
#include <initializer_list>

namespace BlinkenAlgorithms {

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

    uint8_t gamma8(uint8_t v) const {
        static const uint8_t s_gamma8[256] = {
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
            90, 92, 93, 95, 96, 98, 99, 101,
            102, 104, 105, 107, 109, 110, 112, 114,
            115, 117, 119, 120, 122, 124, 126, 127,
            129, 131, 133, 135, 137, 138, 140, 142,
            144, 146, 148, 150, 152, 154, 156, 158,
            160, 162, 164, 167, 169, 171, 173, 175,
            177, 180, 182, 184, 186, 189, 191, 193,
            196, 198, 200, 203, 205, 208, 210, 213,
            215, 218, 220, 223, 225, 228, 231, 233,
            236, 239, 241, 244, 247, 249, 252, 255
        };
        return s_gamma8[v];
    }

protected:
    uint8_t intensity_ = 96;
};

/******************************************************************************/
// Base Reference Strip

template <typename BaseStrip>
class LEDStripRefBase
{
public:
    LEDStripRefBase(BaseStrip& base)
        : base_(base) { }

    size_t size() const {
        return base_.size();
    }

    void show() const {
        return base_.show();
    }
    bool busy() const {
        return base_.busy();
    }

    uint8_t intensity() const {
        return base_.intensity();
    }
    void set_intensity(uint8_t intensity) {
        return base_.set_intensity(intensity);
    }

protected:
    BaseStrip& base_;
};

/******************************************************************************/
// Generic Remappers

template <typename BaseStrip, size_t Repeat>
class RepeatStrip : public LEDStripRefBase<BaseStrip>
{
public:
    using Super = LEDStripRefBase<BaseStrip>;
    using Super::base_;

    RepeatStrip(BaseStrip& base)
        : Super(base) { }

    size_t size() const {
        return base_.size() / Repeat;
    }

    void setPixel(size_t i, const Color& c) {
        for (size_t r = 0; r < Repeat; ++r) {
            base_.setPixel(Repeat * i + r, c);
        }
    }
    void orPixel(size_t i, const Color& c) {
        for (size_t r = 0; r < Repeat; ++r) {
            base_.orPixel(Repeat * i + r, c);
        }
    }
};

template <typename BaseStrip, size_t NumStrips>
class InterleaveStrip
{
public:
    InterleaveStrip(std::initializer_list<BaseStrip*> strips) {
        assert(strips.size() == NumStrips);
        std::copy(strips.begin(), strips.end(), strips_);
    }

    size_t size() { return strips_[0]->size() * NumStrips; }

    void show() const {
        for (size_t s = 0; s < NumStrips; ++s) {
            strips_[s]->show();
        }
    }

    bool busy() const {
        bool is_busy = false;
        for (size_t s = 0; s < NumStrips; ++s) {
            is_busy = is_busy || strips_[s]->busy();
        }
        return is_busy;
    }

    uint8_t intensity() const {
        return strips_[0]->intensity();
    }

    void set_intensity(uint8_t intensity) {
        strips_[0]->set_intensity(intensity);
    }

    void setPixel(size_t i, const Color& c) {
        strips_[i % NumStrips]->setPixel(i / NumStrips, c);
    }

    void orPixel(size_t i, const Color& c) {
        strips_[i % NumStrips]->orPixel(i / NumStrips, c);
    }

    BaseStrip* strips_[NumStrips];
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

} // namespace BlinkenAlgorithms

#endif // !BLINKENALGORITHMS_STRIP_LEDSTRIPBASE_HEADER

/******************************************************************************/
