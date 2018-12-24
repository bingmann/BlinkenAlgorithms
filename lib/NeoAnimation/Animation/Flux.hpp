/*******************************************************************************
 * lib/NeoAnimation/Animation/Flux.hpp
 *
 * Copyright (C) 2016-2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#ifndef NEOANIMATION_ANIMATION_FLUX_HEADER
#define NEOANIMATION_ANIMATION_FLUX_HEADER

#include <NeoAnimation/Color.hpp>
#include <NeoAnimation/Control.hpp>

#include <cmath>
#include <random>

namespace NeoAnimation {

template <typename LEDStrip>
class ColorWipeRGBW
{
public:
    ColorWipeRGBW(LEDStrip& strip) : strip_(strip) { }

    LEDStrip& strip_;

    uint32_t operator () (uint32_t s) {
        size_t strip_size = strip_.size();
        unsigned intensity = strip_.intensity();

        Color r = Color(intensity, 0, 0);
        Color g = Color(0, intensity, 0);
        Color b = Color(0, 0, intensity);
        Color w = Color(intensity);

        for (size_t i = 0; i < strip_size; i += 4) {
            strip_.setPixel((s + i + 0) % strip_size, r);
            strip_.setPixel((s + i + 1) % strip_size, g);
            strip_.setPixel((s + i + 2) % strip_size, b);
            strip_.setPixel((s + i + 3) % strip_size, w);
        }

        return 200000;
    }
};

template <typename LEDStrip>
class ColorWipeTwoSine
{
public:
    ColorWipeTwoSine(LEDStrip& strip) : strip_(strip) { }

    LEDStrip& strip_;

    uint32_t operator () (uint32_t s) {
        size_t strip_size = strip_.size();
        unsigned intensity = strip_.intensity();

        Color c0 = Color(0, 0, intensity);
        Color c1 = Color(0, intensity, 0);

        static float xf;
        if (s == 0)
            xf = 0;

        xf += 0.2 + 1.5 * sin(static_cast<float>(s) / M_PI / 4.0);
        size_t x = static_cast<size_t>(xf);

        for (size_t i = 0; i < strip_size; i += 6) {
            strip_.setPixel((i + x + 0 + strip_size) % strip_size, c0);
            strip_.setPixel((i + x + 1 + strip_size) % strip_size, c0);
            strip_.setPixel((i + x + 2 + strip_size) % strip_size, c0);
            strip_.setPixel((i + x + 3 + strip_size) % strip_size, c1);
            strip_.setPixel((i + x + 4 + strip_size) % strip_size, c1);
            strip_.setPixel((i + x + 5 + strip_size) % strip_size, c1);
        }

        return 50000;
    }
};

template <typename LEDStrip>
class WheelColorTest
{
public:
    WheelColorTest(LEDStrip& strip) : strip_(strip) { }

    LEDStrip& strip_;

    uint32_t operator () (uint32_t /* s */) {
        size_t strip_size = strip_.size();
        unsigned intensity = strip_.intensity();

        for (size_t i = 0; i < strip_size; ++i) {
            strip_.setPixel(i, WheelColor(i, intensity));
        }

        return 20000;
    }
};

template <typename LEDStrip>
class HSVColorTest
{
public:
    HSVColorTest(LEDStrip& strip) : strip_(strip) { }

    LEDStrip& strip_;

    uint32_t operator () (uint32_t s) {
        size_t strip_size = strip_.size();
        unsigned intensity = strip_.intensity();

        // for (size_t i = 0; i < strip_size; ++i) {
        //     strip_.setPixel(i, HSVColor(i + s, 255, intensity));
        // }
        Color c = HSVColor((s * 10) % HSV_HUE_MAX, 255, intensity);
        for (size_t i = 0; i < strip_size; ++i) {
            strip_.setPixel(i, c);
        }

        return 20000;
    }
};

template <typename LEDStrip>
class HSVColorWheel
{
public:
    HSVColorWheel(LEDStrip& strip) : strip_(strip) { }

    LEDStrip& strip_;

    uint32_t operator () (uint32_t s) {
        size_t strip_size = strip_.size();
        unsigned intensity = strip_.intensity();

        for (size_t i = 0; i < strip_size; ++i) {
            size_t j = i / 300;
            strip_.setPixel(
                i, HSVColor((s + j * HSV_HUE_MAX / 5) % HSV_HUE_MAX,
                            255, intensity));
        }

        return 200;
    }
};

template <typename LEDStrip>
class Strobo1
{
public:
    Strobo1(LEDStrip& strip) : strip_(strip) { }

    LEDStrip& strip_;

    uint32_t operator () (uint32_t s) {
        size_t strip_size = strip_.size();
        unsigned intensity = strip_.intensity();

        for (size_t i = 0; i < strip_size; ++i)
            strip_.setPixel(i, 0);
        strip_.show();

        for (uint32_t s = 0; ; ++s) {
            for (size_t i = 0; i < strip_size; ++i) {
                strip_.setPixel(i, Color(intensity));
            }
            strip_.show();
            strip_.delay(25);

            for (size_t i = 0; i < strip_size; ++i) {
                strip_.setPixel(i, Color(0));
            }
            strip_.show();
            strip_.delay(25);
        }

        return 0;
    }
};

/******************************************************************************/

template <typename LEDStrip>
class SparkleWhite
{
public:
    SparkleWhite(LEDStrip& strip, size_t speed = 100, size_t density = 10)
        : strip_(strip),
          seed_(static_cast<uint32_t>(random(10000000))),
          speed_(speed), density_(density) { }

    LEDStrip& strip_;

    size_t pix = 4;

    uint32_t seed_;

    size_t speed_;
    size_t density_;

    std::default_random_engine rng1{ seed_ };
    std::default_random_engine rng2{ seed_ };

    uint32_t operator () (uint32_t s) {
        size_t strip_size = strip_.size();
        Color w = Color(strip_.intensity());

        if (s % 2 == 0) {
            strip_.setPixel(rng1() % strip_size, w);
        }
        else {
            if (pix >= strip_size / density_)
                strip_.setPixel(rng2() % strip_size, 0);
            else
                ++pix;
        }

        return speed_;
    }
};

template <typename LEDStrip>
class SparkleRGB
{
public:
    SparkleRGB(LEDStrip& strip, size_t speed = 100, size_t density = 10)
        : strip_(strip),
          seed_(static_cast<uint32_t>(random(10000000))),
          speed_(speed), density_(density) { }

    LEDStrip& strip_;

    size_t pix = 4;

    uint32_t seed_;

    size_t speed_;
    size_t density_;

    std::default_random_engine rng1{ seed_ };
    std::default_random_engine rng2{ seed_ };
    std::default_random_engine rng3{ seed_ };

    uint32_t operator () (uint32_t s) {
        size_t strip_size = strip_.size();
        unsigned intensity = strip_.intensity();

        if (s % 2 == 0) {
            strip_.setPixel(
                rng1() % strip_size, WheelColor(rng3(), intensity));
        }
        else {
            if (pix >= strip_size / density_)
                strip_.setPixel(rng2() % strip_size, 0);
            else
                ++pix;
        }

        return speed_;
    }
};

template <typename Strip>
void setPixelFireColor(Strip& strip, int index, uint8_t temperature) {
    // Scale 'heat' down from 0-255 to 0-191
    uint8_t t192 = round((temperature / 255.0) * 191);

    // calculate ramp up from
    uint8_t heatramp = t192 & 0x3F; // 0..63
    heatramp <<= 2;                 // scale up to 0..252

    // figure out which third of the spectrum we're in:
    Color c = Color(0);
    if (t192 > 0x80) { // hottest
        c = Color(255, 255, heatramp);
    }
    else if (t192 > 0x40) { // middle
        c = Color(255, heatramp, 0);
    }
    else {  // coolest
        c = Color(heatramp, 0, 0);
    }

    strip.setPixel(index, c);
}

template <typename Strip>
void setPixelFireIceColor(Strip& strip, int index,
                          uint8_t heat, uint8_t ice, uint8_t intensity) {
    // Scale 'heat' down from 0-255 to 0-191
    uint8_t h192 = round((heat / 255.0) * 191);

    // calculate ramp up from
    uint8_t heatramp = h192 & 0x3F; // 0..63
    heatramp <<= 2;                 // scale up to 0..252

    uint8_t i192 = round((ice / 255.0) * 191);
    uint8_t iceramp = i192 & 0x3F; // 0..63
    iceramp <<= 2;                 // scale up to 0..252

    uint16_t r = 0, g = 0, b = 0;

    // figure out which third of the spectrum we're in:
    if (h192 > 0x80) { // hottest
        r += 255;
        g += 255;
        b += heatramp;
    }
    else if (h192 > 0x40) { // middle
        r += 255;
        g += heatramp;
    }
    else {  // coolest
        r += heatramp;
    }

    if (i192 > 0x80) { // hottest
        r += iceramp;
        g += 255;
        b += 255;
    }
    else if (i192 > 0x40) { // middle
        b += 255;
        g += iceramp;
    }
    else {  // coolest
        b += iceramp;
    }

    if (r >= 256)
        r = 255;
    if (g >= 256)
        g = 255;
    if (b >= 256)
        b = 255;

    r = r * intensity / 255;
    g = g * intensity / 255;
    b = b * intensity / 255;

    strip.setPixel(index, Color(r, g, b));
}

template <typename LEDStrip>
class Fire
{
public:
    Fire(LEDStrip& strip, size_t cooling = 20, size_t sparking = 160)
        : strip_(strip), cooling_(cooling), sparking_(sparking) {
        strip_size = strip.size();
        heat.resize(strip_size);
    }

    LEDStrip& strip_;

    uint32_t operator () (uint32_t /* s */) {
        // Step 1. Cool down every cell a little
        for (size_t i = 0; i < strip_size; i++) {
            size_t cooldown = random(0, ((cooling_ * 10) / strip_size) + 2);

            if (cooldown > heat[i]) {
                heat[i] = 0;
            }
            else {
                heat[i] = heat[i] - cooldown;
            }
        }

        // Step 2. Heat from each cell drifts 'up' and diffuses a little
        for (size_t k = strip_size - 1; k >= 2; k--) {
            heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
        }

        // Step 3. Randomly ignite new 'sparks' near the bottom
        if (random(255) < sparking_) {
            size_t y = random(7);
            heat[y] = heat[y] + random(160, 255);
            // heat[y] = random(160,255);
        }

        // Step 4. Convert heat to LED colors
        for (size_t j = 0; j < strip_size; j++) {
            setPixelFireColor(strip_, j, heat[j]);
            // setPixelFireIceColor(strip_, j, 0, heat[j]);
        }

        return 10000;
    }

private:
    size_t cooling_;
    size_t sparking_;

    std::vector<uint8_t> heat;
    size_t strip_size;
};

template <typename LEDStrip>
class FireIce
{
public:
    FireIce(LEDStrip& strip, size_t cooling = 20, size_t sparking = 160)
        : strip_(strip), cooling_(cooling), sparking_(sparking) {
        strip_size = strip_.size();
        heat.resize(strip_size);
        ice.resize(strip_size);
    }

    uint32_t operator () (uint32_t /* s */) {
        // Step 1. Cool down every cell a little
        for (size_t i = 0; i < strip_size; i++) {
            size_t cooldown = random(0, ((cooling_ * 10) / strip_size) + 2);

            if (cooldown > heat[i]) {
                heat[i] = 0;
            }
            else {
                heat[i] = heat[i] - cooldown;
            }
        }
        for (size_t i = 0; i < strip_size; i++) {
            size_t cooldown = random(0, ((cooling_ * 10) / strip_size) + 2);

            if (cooldown > ice[i]) {
                ice[i] = 0;
            }
            else {
                ice[i] = ice[i] - cooldown;
            }
        }

        // Step 2. Heat from each cell drifts 'up' and diffuses a little
        for (size_t k = strip_size - 1; k >= 2; k--) {
            heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
        }
        for (size_t k = strip_size - 1; k >= 2; k--) {
            ice[k] = (ice[k - 1] + ice[k - 2] + ice[k - 2]) / 3;
        }

        // Step 3. Randomly ignite new 'sparks' near the bottom
        if (random(255) < sparking_) {
            size_t y = random(7);
            heat[y] = heat[y] + random(160, 255);
            // heat[y] = random(160,255);
        }
        if (random(255) < sparking_) {
            size_t y = random(7);
            ice[y] = ice[y] + random(160, 255);
            // ice[y] = random(160,255);
        }

        // Step 4. Convert heat to LED colors
        for (size_t j = 0; j < strip_size; j++) {
            size_t h = heat[strip_size - j - 1];
            size_t e = ice[j];
            // setPixelFireIceColor(
            //     strip, strip_size - j - 1, heat[strip_size - j - 1], ice[j]);

            if (h < e)
                h = 0;
            else
                e = 0;

            setPixelFireIceColor(strip_, strip_size - j - 1,
                                 h, e, strip_.intensity());
        }

        return 10000;
    }

    LEDStrip& strip_;

private:
    size_t cooling_;
    size_t sparking_;

    std::vector<uint8_t> heat, ice;
    size_t strip_size;
};

template <typename LEDStrip>
class SprayColor
{
public:
    struct Pixi {
        unsigned part;
        float pos;
        float speed;
        Color color = Color(0);
    };

    LEDStrip& strip_;

    std::vector<Pixi> pixis_;
    size_t free_ = 0;

    unsigned origin_;
    uint32_t speed_;
    size_t density_ratio_;

    SprayColor(LEDStrip& strip, unsigned origin = 0,
               uint32_t speed = 30000, size_t density_ratio = 5)
        : strip_(strip), origin_(origin), speed_(speed),
          density_ratio_(density_ratio) {
        pixis_.resize(strip.size() / density_ratio_);
    }

    uint32_t operator () (uint32_t /* s */) {
        size_t strip_size = strip_.size();
        size_t strip_parts = 1;

        if (random(density_ratio_) == 0) {
            // make new pixi
            if (free_ < pixis_.size()) {
                Pixi& p = pixis_[free_++];

                p.part = random(strip_parts);
                int rndsel = random(2);
                if (origin_ == 0 || (origin_ == 2 && rndsel % 2 == 0)) {
                    p.pos = 0;
                    p.speed = 1.0 + random(10) / 10.0;
                }
                else if (origin_ == 1 || (origin_ == 2 && rndsel % 2 == 1)) {
                    p.pos = strip_size - 1;
                    p.speed = -(1.0 + random(10) / 10.0);
                }

                p.color = WheelColor(random(256), strip_.intensity());
            }
        }

        for (size_t i = 0; i < strip_size; ++i) {
            strip_.setPixel(i, 0);
        }
        for (size_t i = 0; i < free_; ++i) {
            Pixi& p = pixis_[i];
            strip_.setPixel(p.part * strip_size + p.pos, p.color);
            p.pos += p.speed;
            if (p.pos < 0 || p.pos >= strip_size) {
                if (i + 1 < free_) {
                    // swap in last Pixi
                    pixis_[i] = std::move(pixis_[free_ - 1]);
                    --free_;
                    --i;
                }
                else {
                    // no remaining Pixi
                    --free_;
                }
            }
        }

        return speed_;
    }
};

template <typename LEDStrip>
class Fireworks
{
public:
    static const size_t cracks = 24;
    struct Pixi {
        bool on;
        unsigned part;
        float pos;
        float speed[cracks];
        size_t ts, ts_end;
        Color color = Color(0);
    };

    LEDStrip& strip_;

    std::vector<Pixi> pixis_;

    std::default_random_engine rng;
    std::normal_distribution<float> norm;

    Fireworks(LEDStrip& strip)
        : strip_(strip),
          rng(/* seed */ static_cast<uint32_t>(random(10000000))) {
        pixis_.resize(10);
    }

    uint32_t operator () (uint32_t /* s */) {
        size_t strip_size = strip_.size();
        size_t strip_parts = 1;

        if (random(12) < 4)
        {
            // make new pixi
            size_t j;
            for (j = 0; j < pixis_.size(); ++j) {
                if (pixis_[j].on == false)
                    break;
            }
            if (j < pixis_.size()) {
                pixis_[j].on = true;
                pixis_[j].part = random(strip_parts);
                pixis_[j].pos = random(strip_size);
                float size = 20.0 + random(10000) / 1000.0;
                for (size_t k = 0; k < cracks; ++k) {
                    pixis_[j].speed[k] = norm(rng) * size;
                }
                pixis_[j].ts = 0;
                pixis_[j].ts_end = 80 + random(160);
                pixis_[j].color =
                    WheelColor(random(256), strip_.intensity());
            }
        }

        for (size_t i = 0; i < strip_size; ++i) {
            strip_.setPixel(i, 0);
        }
        for (size_t i = 0; i < pixis_.size(); ++i) {
            if (!pixis_[i].on)
                continue;

            Pixi& p = pixis_[i];
            for (size_t k = 0; k < cracks; ++k) {
                ssize_t x = p.pos + p.speed[k] * sin(p.ts * M_PI / p.ts_end);
                if (x >= 0 && x < ssize_t(strip_size))
                    strip_.orPixel(p.part * strip_size + x, p.color);
            }

            p.ts++;
            if (p.ts >= p.ts_end)
                p.on = false;
        }

        return 500;
    }
};

template <typename LEDStrip>
class Starlight
{
public:
    Starlight(LEDStrip& strip) : strip_(strip) {
        strip_size = strip.size();

        for (size_t i = 0; i < max_blinks; ++i) {
            blinks[i] = strip_size;
        }
    }

    LEDStrip& strip_;

    uint32_t operator () (uint32_t s) {
        unsigned intensity = strip_.intensity();

        // Color w = Color(32, 0, 0, 100);
        Color w = Color(0, 0, 0, intensity);
        // Color w = Color(0, 0, 0, 2);

        for (size_t i = 0; i < strip_size; ++i)
            strip_.setPixel(i, w);

        if (rng() % 128 < 16 && 0) {
            // add new blink
            for (size_t i = 0; i < max_blinks; ++i) {
                if (blinks[i] >= strip_size) {
                    blinks[i] = rng() % strip_size;
                    blinks_age[i] = 0;
                    break;
                }
            }
        }

        for (size_t i = 0; i < max_blinks; ++i) {
            if (blinks[i] >= strip_size)
                continue;

            if (dist(rng) * 100 < blinks_age[i]) {
                size_t v = ldist(rng) * 10;
                strip_.setPixel(blinks[i], v);
            }

            blinks_age[i]++;
            if (blinks_age[i] > 200)
                blinks[i] = strip_size;
        }

        return 50000;
    }

protected:
    static const size_t max_blinks = 8;
    size_t strip_size;

    std::default_random_engine rng { 12345 };
    std::normal_distribution<float> dist { 1.0, 1.0 };
    std::lognormal_distribution<float> ldist { 0.0, 0.5 };

    size_t blinks[max_blinks];
    size_t blinks_age[max_blinks];
    size_t blinks_hue[max_blinks];
};

} // namespace NeoAnimation

#endif // !NEOANIMATION_ANIMATION_FLUX_HEADER

/******************************************************************************/
