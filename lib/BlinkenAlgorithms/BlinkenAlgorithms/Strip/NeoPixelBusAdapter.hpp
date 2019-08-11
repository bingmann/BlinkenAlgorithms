/*******************************************************************************
 * lib/BlinkenAlgorithms/BlinkenAlgorithms/Strip/NeoPixelBusAdapter.hpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#ifndef BLINKENALGORITHMS_STRIP_NEOPIXELBUSADAPTER_HEADER
#define BLINKENALGORITHMS_STRIP_NEOPIXELBUSADAPTER_HEADER

#include <BlinkenAlgorithms/Strip/LEDStripBase.hpp>

#include <NeoPixelBus.h>

namespace BlinkenAlgorithms {

template <typename NeoPixelBus>
class NeoPixelBusAdapter : public LEDStripBase
{
public:
    explicit NeoPixelBusAdapter(NeoPixelBus& strip)
        : LEDStripBase(), strip_(strip) { }

    size_t size() const {
        return strip_.PixelCount();
    }

    void show() const {
        return strip_.Show();
    }

    bool busy() {
        return !strip_.CanShow();
    }

    void setPixel(size_t i, const Color& c) {
        setPixelRaw(i, Color(gamma8(c.r), gamma8(c.g),
                             gamma8(c.b), gamma8(c.w)));
    }

    Color getPixel(size_t i) const {
        RgbwColor c = strip_.GetPixelColor(i);
        return Color(c.R, c.G, c.B, c.W);
    }

    void orPixel(size_t i, const Color& c) {
        Color c1 = getPixel(i);
        Color c2(gamma8(c.r), gamma8(c.g), gamma8(c.b), gamma8(c.w));
        return setPixelRaw(i, c1 | c2);
    }

    void addPixel(size_t i, const Color& c) {
        Color c1 = getPixel(i);
        Color c2(gamma8(c.r), gamma8(c.g), gamma8(c.b), gamma8(c.w));
        return setPixelRaw(i, c1 + c2);
    }

private:
    NeoPixelBus& strip_;

    void setPixelRaw(size_t i, const Color& c) {
        strip_.SetPixelColor(i, RgbwColor(c.r, c.g, c.b, c.w));
    }
};

} // namespace BlinkenAlgorithms

#endif // !BLINKENALGORITHMS_STRIP_NEOPIXELBUSADAPTER_HEADER

/******************************************************************************/
