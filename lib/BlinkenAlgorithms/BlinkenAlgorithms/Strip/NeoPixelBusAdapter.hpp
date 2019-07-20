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
        strip_.SetPixelColor(i, RgbwColor(c.r, c.g, c.b, c.w));
    }

    void orPixel(size_t i, const Color& c) {
        RgbwColor b = strip_.GetPixelColor(i);
        strip_.SetPixelColor(
            i, RgbwColor(c.r | b.R, c.g | b.G, c.b | b.B, c.w | b.W));
    }

private:
    NeoPixelBus& strip_;
};

} // namespace BlinkenAlgorithms

#endif // !BLINKENALGORITHMS_STRIP_NEOPIXELBUSADAPTER_HEADER

/******************************************************************************/
