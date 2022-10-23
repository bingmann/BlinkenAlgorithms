/*******************************************************************************
 * lib/BlinkenAlgorithms/BlinkenAlgorithms/Animation/Flux.hpp
 *
 * Copyright (C) 2016-2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#ifndef BLINKENALGORITHMS_ANIMATION_FLUX_HEADER
#define BLINKENALGORITHMS_ANIMATION_FLUX_HEADER

#include <BlinkenAlgorithms/Color.hpp>
#include <BlinkenAlgorithms/Control.hpp>
#include <BlinkenAlgorithms/RunAnimation.hpp>

#include <cmath>
#include <random>

namespace BlinkenAlgorithms {

/*!
 * 1 Pixel RGBW shifts by one pixel each frame.
 */
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

/*!
 * 3 Pixel in two colors shift in a sine wave
 */
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

        xf += 0.2 + 3.5 * sin(static_cast<float>(s) / M_PI / 16.0);
        size_t x = static_cast<size_t>(xf);

        for (size_t i = 0; i < strip_size; i += 6) {
            strip_.setPixel((i + x + 0 + strip_size) % strip_size, c0);
            strip_.setPixel((i + x + 1 + strip_size) % strip_size, c0);
            strip_.setPixel((i + x + 2 + strip_size) % strip_size, c0);
            strip_.setPixel((i + x + 3 + strip_size) % strip_size, c1);
            strip_.setPixel((i + x + 4 + strip_size) % strip_size, c1);
            strip_.setPixel((i + x + 5 + strip_size) % strip_size, c1);
        }

        return 100000;
    }
};

/*!
 * Display the colors of the WheelColor method for testing
 */
template <typename LEDStrip>
class WheelColorTest
{
public:
    WheelColorTest(LEDStrip& strip) : strip_(strip) { }

    LEDStrip& strip_;

    uint32_t operator () (uint32_t /* s */) {
        size_t strip_size = strip_.size();
        unsigned intensity = std::min<uint8_t>(strip_.intensity(), 255u);

        for (size_t i = 0; i < strip_size; ++i) {
            strip_.setPixel(i, WheelColor(i, intensity));
        }

        return 20000;
    }
};

/*!
 * Cycle the colors of the WheelColor method in each frame.
 */
template <typename LEDStrip>
class WheelColorWheel
{
public:
    WheelColorWheel(LEDStrip& strip, size_t speed = 2000)
        : strip_(strip), speed_(speed) { }

    LEDStrip& strip_;
    size_t speed_;

    uint32_t operator () (uint32_t s) {
        size_t strip_size = strip_.size();
        unsigned intensity = std::min<uint8_t>(strip_.intensity(), 255u);

        for (size_t i = 0; i < strip_size; ++i) {
            size_t j = i / 300;
            strip_.setPixel(
                i, WheelColor((s + j * 256 / 5) % 256, intensity));
        }

        return speed_;
    }
};

/*!
 * Display the colors of the HSVColor method for testing
 */
template <typename LEDStrip>
class HSVColorTest
{
public:
    HSVColorTest(LEDStrip& strip, size_t speed = 20000)
        : strip_(strip), speed_(speed) { }

    LEDStrip& strip_;
    size_t speed_;

    uint32_t operator () (uint32_t s) {
        size_t strip_size = strip_.size();
        unsigned intensity = std::min<uint8_t>(strip_.intensity(), 255u);

        for (size_t i = 0; i < strip_size; ++i) {
            Color c = HSVColor(i % HSV_HUE_MAX, 255, intensity);
            strip_.setPixel(i, c);
        }

        return speed_;
    }
};

/*!
 * Cycle the colors of the HSVColor method in each frame.
 */
template <typename LEDStrip>
class HSVColorWheel
{
public:
    HSVColorWheel(LEDStrip& strip, size_t speed = 200)
        : strip_(strip), speed_(speed) { }

    LEDStrip& strip_;
    size_t speed_;

    HSVCache hsv_;

    uint32_t operator () (uint32_t s) {
        size_t strip_size = strip_.size();
        unsigned intensity = std::min<uint8_t>(strip_.intensity(), 255u);

        for (size_t i = 0; i < strip_size; ++i) {
            size_t j = i / 300;
            strip_.setPixel(
                i, hsv_.get((s + j * HSV_HUE_MAX / 5) % HSV_HUE_MAX,
                            255, intensity));
        }

        return speed_;
    }
};

/******************************************************************************/

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

        for (uint32_t s = 0; s < 4000 / 80; ++s) {
            for (size_t i = 0; i < strip_size; ++i) {
                strip_.setPixel(i, Color(intensity));
            }
            strip_.show();
            delay(40);

            for (size_t i = 0; i < strip_size; ++i) {
                strip_.setPixel(i, Color(0));
            }
            strip_.show();
            delay(40);
        }

        return EndAnimation;
    }
};

template <typename LEDStrip>
class Strobo2
{
public:
    Strobo2(LEDStrip& strip) : strip_(strip) { }

    LEDStrip& strip_;

    uint32_t operator () (uint32_t s) {
        size_t strip_size = strip_.size();
        unsigned intensity = strip_.intensity();

        for (size_t i = 0; i < strip_size; ++i)
            strip_.setPixel(i, 0);
        strip_.show();

        for (uint32_t r = 0; r < 3; ++r) {
            for (uint32_t s = 0; s < 4000 / 100 / 3; ++s) {
                for (size_t i = 0; i < strip_size; ++i) {
                    strip_.setPixel(i, Color(intensity));
                }
                strip_.show();
                delay(60);

                for (size_t i = 0; i < strip_size; ++i) {
                    strip_.setPixel(i, Color(0));
                }
                strip_.show();
                delay(60);
            }

            delay(600);
        }

        return EndAnimation;
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

    size_t pix_ = 0;

    uint32_t seed_;

    size_t speed_;
    size_t density_;

    std::default_random_engine rng1_{ seed_ };
    std::default_random_engine rng2_{ seed_ };

    uint32_t operator () (uint32_t s) {
        size_t strip_size = strip_.size();
        Color w = Color(strip_.intensity());

        if (s % 2 == 0) {
            strip_.setPixel(rng1_() % strip_size, w);
        }
        else {
            if (pix_ >= strip_size / density_)
                strip_.setPixel(rng2_() % strip_size, 0);
            else
                ++pix_;
        }

        return speed_;
    }
};

template <typename LEDStrip, bool TrueHSV = false>
class SparkleRGB
{
public:
    SparkleRGB(LEDStrip& strip, size_t speed = 100, size_t density = 10,
               size_t multi = 1)
        : strip_(strip),
          seed_(static_cast<uint32_t>(random(10000000))),
          speed_(speed), density_(density), multi_(multi) { }

    LEDStrip& strip_;

    size_t pix_ = 0;

    uint32_t seed_;

    size_t speed_;
    size_t density_;
    size_t multi_;

    std::default_random_engine rng1_{ seed_ };
    std::default_random_engine rng2_{ seed_ };
    std::default_random_engine rng3_{ seed_ };

    uint32_t operator () (uint32_t) {
        size_t strip_size = strip_.size();
        unsigned intensity = strip_.intensity();

        for (size_t r = 0; r < multi_; ++r) {
            strip_.setPixel(
                rng1_() % strip_size,
                TrueHSV
                ? HSVColor(rng3_() % HSV_HUE_MAX, 255,
                           intensity)
                : WheelColor(rng3_(), intensity));

            if (pix_ >= strip_size / density_)
                strip_.setPixel(rng2_() % strip_size, Color(0));
            else
                ++pix_;
        }

        return speed_;
    }
};

/******************************************************************************/

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
        strip_size_ = strip.size();
        heat_.resize(strip_size_);
    }

    LEDStrip& strip_;

    uint32_t operator () (uint32_t /* s */) {
        // Step 1. Cool down every cell a little
        for (size_t i = 0; i < strip_size_; i++) {
            size_t cooldown = random(0, ((cooling_ * 10) / strip_size_) + 2);

            if (cooldown > heat_[i]) {
                heat_[i] = 0;
            }
            else {
                heat_[i] = heat_[i] - cooldown;
            }
        }

        // Step 2. Heat from each cell drifts 'up' and diffuses a little
        for (size_t k = strip_size_ - 1; k >= 2; k--) {
            heat_[k] = (heat_[k - 1] + heat_[k - 2] + heat_[k - 2]) / 3;
        }

        // Step 3. Randomly ignite new 'sparks' near the bottom
        if (random(255) < sparking_) {
            size_t y = random(7);
            heat_[y] = heat_[y] + random(160, 255);
            // heat[y] = random(160,255);
        }

        // Step 4. Convert heat to LED colors
        for (size_t j = 0; j < strip_size_; j++) {
            setPixelFireColor(strip_, j, heat_[j]);
            // setPixelFireIceColor(strip_, j, 0, heat[j]);
        }

        return 10000;
    }

private:
    size_t cooling_;
    size_t sparking_;

    std::vector<uint8_t> heat_;
    size_t strip_size_;
};

template <typename LEDStrip>
class FireIce
{
public:
    FireIce(LEDStrip& strip, size_t cooling = 20, size_t sparking = 160)
        : strip_(strip), cooling_(cooling), sparking_(sparking) {
        strip_size_ = strip_.size();
        heat_.resize(strip_size_);
        cold_.resize(strip_size_);
    }

    uint32_t operator () (uint32_t /* s */) {
        // Step 1. Cool down every cell a little
        for (size_t i = 0; i < strip_size_; i++) {
            size_t cooldown = random(0, ((cooling_ * 10) / strip_size_) + 2);

            if (cooldown > heat_[i]) {
                heat_[i] = 0;
            }
            else {
                heat_[i] = heat_[i] - cooldown;
            }
        }
        for (size_t i = 0; i < strip_size_; i++) {
            size_t cooldown = random(0, ((cooling_ * 10) / strip_size_) + 2);

            if (cooldown > cold_[i]) {
                cold_[i] = 0;
            }
            else {
                cold_[i] = cold_[i] - cooldown;
            }
        }

        // Step 2. Heat from each cell drifts 'up' and diffuses a little
        for (size_t k = strip_size_ - 1; k >= 2; k--) {
            heat_[k] = (heat_[k - 1] + heat_[k - 2] + heat_[k - 2]) / 3;
        }
        for (size_t k = strip_size_ - 1; k >= 2; k--) {
            cold_[k] = (cold_[k - 1] + cold_[k - 2] + cold_[k - 2]) / 3;
        }

        // Step 3. Randomly ignite new 'sparks' near the bottom
        if (random(255) < sparking_) {
            size_t y = random(7);
            heat_[y] = heat_[y] + random(160, 255);
            // heat[y] = random(160,255);
        }
        if (random(255) < sparking_) {
            size_t y = random(7);
            cold_[y] = cold_[y] + random(160, 255);
            // ice[y] = random(160,255);
        }

        // Step 4. Convert heat to LED colors
        for (size_t j = 0; j < strip_size_; j++) {
            size_t h = heat_[strip_size_ - j - 1];
            size_t e = cold_[j];
            // setPixelFireIceColor(
            //     strip, strip_size - j - 1, heat[strip_size - j - 1], ice[j]);

            if (h < e)
                h = 0;
            else
                e = 0;

            setPixelFireIceColor(strip_, strip_size_ - j - 1,
                                 h, e, strip_.intensity());
        }

        return 10000;
    }

    LEDStrip& strip_;

private:
    size_t cooling_;
    size_t sparking_;

    std::vector<uint8_t> heat_, cold_;
    size_t strip_size_;
};

template <typename LEDStrip, bool TrueHSV = false>
class SprayColor
{
public:
    struct Pixi {
        unsigned part;
        float pos;
        float speed;
        Color color = Color(0);
        uint16_t hue;
        uint8_t intensity;
    };

    LEDStrip& strip_;

    std::vector<Pixi> pixis_;
    size_t free_ = 0;

    unsigned origin_;
    uint32_t speed_;
    size_t density_ratio_;

    SprayColor(LEDStrip& strip, unsigned origin = 0,
               uint32_t speed = 30000, size_t density_ratio = 6)
        : strip_(strip), origin_(origin), speed_(speed),
          density_ratio_(density_ratio) {
        pixis_.resize(strip.size() / density_ratio_);
    }

    uint32_t operator () (uint32_t /* s */) {
        size_t strip_size = strip_.size();
        size_t strip_parts = 1;

        if (random(density_ratio_) <= 2) {
            // make new pixi
            if (free_ < pixis_.size()) {
                Pixi& p = pixis_[free_++];

                p.part = random(strip_parts);
                int rndsel = random(2);
                if (origin_ == 0 || (origin_ == 2 && rndsel % 2 == 0)) {
                    p.pos = random(strip_size / 4) - strip_size / 8.0;
                    if (p.pos < 0)
                        p.pos = 0;
                    p.speed = 1.0 + random(10) / 10.0;
                }
                else if (origin_ == 1 || (origin_ == 2 && rndsel % 2 == 1)) {
                    p.pos = strip_size - 1 -
                            (random(strip_size / 4) - strip_size / 8.0);
                    if (p.pos > strip_size - 1)
                        p.pos = strip_size - 1;
                    p.speed = -(1.0 + random(10) / 10.0);
                }

                p.hue = TrueHSV ? random(HSV_HUE_MAX) : random(256);
                p.intensity = strip_.intensity();
                p.color = TrueHSV ? HSVColor(p.hue, 255, p.intensity)
                          : WheelColor(p.hue, p.intensity);
            }
        }

        for (size_t i = 0; i < strip_size; ++i) {
            strip_.setPixel(i, 0);
        }
        uint8_t intensity = strip_.intensity();
        for (size_t i = 0; i < free_; ++i) {
            Pixi& p = pixis_[i];
            if (p.intensity != intensity) {
                p.intensity = intensity;
                p.color = WheelColor(p.hue, p.intensity);
            }
            strip_.addPixel(p.part * strip_size + p.pos, p.color);
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
    static const size_t cracks = 16;

    struct Pixi {
        bool on;
        unsigned part;
        float pos;
        float speed[cracks];
        size_t ts, ts_end;
        Color color = Color(0);
        uint8_t hue;
        uint8_t intensity;
    };

    LEDStrip& strip_;

    std::vector<Pixi> pixis_;

    std::default_random_engine rng_;
    std::normal_distribution<float> norm_;

    Fireworks(LEDStrip& strip)
        : strip_(strip),
          rng_(/* seed */ static_cast<uint32_t>(random(10000000))) {
        pixis_.resize(strip_.size() / cracks / 10);
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
                Pixi& p = pixis_[j];

                p.on = true;
                p.part = random(strip_parts);
                p.pos = random(strip_size);
                float size = 10.0 + random(10000) / 1000.0;
                for (size_t k = 0; k < cracks; ++k) {
                    p.speed[k] = norm_(rng_) * size;
                }
                p.ts = 0;
                p.ts_end = 40 + random(120);
                p.hue = random(HSV_HUE_MAX);
                p.intensity = strip_.intensity();
                p.color = WheelColor(p.hue, p.intensity);
            }
        }

        for (size_t i = 0; i < strip_size; ++i) {
            strip_.setPixel(i, 0);
        }
        uint8_t intensity = strip_.intensity();
        for (size_t i = 0; i < pixis_.size(); ++i) {
            if (!pixis_[i].on)
                continue;

            Pixi& p = pixis_[i];
            if (p.intensity != intensity) {
                p.intensity = intensity;
                p.color = HSVColor(p.hue, 255, p.intensity);
            }
            for (size_t k = 0; k < cracks; ++k) {
                ssize_t x = p.pos + p.speed[k] * sin(p.ts * M_PI / p.ts_end);
                if (x >= 0 && x < ssize_t(strip_size))
                    strip_.orPixel(p.part * strip_size + x, p.color);
            }

            p.ts++;
            if (p.ts >= p.ts_end)
                p.on = false;
        }

        return 50;
    }
};

/******************************************************************************/

template <typename LEDStrip, bool TrueHSV = false>
struct KnightSnakes {

    KnightSnakes(LEDStrip& strip, size_t speed, size_t max_snakes)
        : strip_(strip), speed_(speed), max_snakes_(max_snakes) {
        snakes_.resize(max_snakes);
    }

    LEDStrip& strip_;
    size_t speed_;
    size_t max_snakes_;

    size_t num_snakes = 0;

    struct Snake {
        double speed;
        uint32_t start;
        uint8_t length;
        uint16_t hue;
    };
    std::vector<Snake> snakes_;

    size_t operator () (uint32_t s) {
        size_t strip_size = strip_.size();

        if (s % 8 == 0 && num_snakes < max_snakes_) {
            Snake& sk = snakes_[num_snakes++];
            sk.speed = 0.5 + random(32) / 16.0;
            if (TrueHSV)
                sk.hue = random(HSV_HUE_MAX);
            else
                sk.hue = random(255);
            sk.start = s * sk.speed + random(strip_size);
            sk.length = 8 + random(32);
        }

        for (size_t i = 0; i < strip_size; ++i)
            strip_.setPixel(i, 0);

        for (size_t i = 0; i < num_snakes; ++i) {
            Snake& sk = snakes_[i];

            for (size_t j = 0; j < sk.length; ++j) {
                size_t x =
                    ((size_t)(s * sk.speed) - sk.start + j) % (2 * strip_size);

                // Color col = sk.color;
                // Color col(sk.color.r * j / sk.length,
                //           sk.color.g * j / sk.length,
                //           sk.color.b * j / sk.length);
                Color col;
                if (TrueHSV) {
                    col = HSVColor(
                        sk.hue, 255, strip_.intensity() * j / sk.length);
                }
                else {
                    col = WheelColor(
                        sk.hue, strip_.intensity() * j / sk.length);
                }

                if (x < strip_size)
                    strip_.addPixel(x, col);
                else
                    strip_.addPixel(2 * strip_size - x, col);
            }
        }

        return speed_;
    }
};

/******************************************************************************/

template <typename LEDStrip>
class PulseColor
{
public:
    struct Pixi {
        bool on;
        unsigned part;
        float pos;
        float speed;
        float size;
        size_t ts, ts_end;
        uint16_t hue;
    };

    LEDStrip& strip_;

    std::vector<Pixi> pixis_;
    size_t pixis_count_ = 0;

    std::default_random_engine rng_;
    std::normal_distribution<float> norm_;

    PulseColor(LEDStrip& strip, size_t density_ratio = 6)
        : strip_(strip),
          rng_(/* seed */ static_cast<uint32_t>(random(10000000))) {
        pixis_.resize(strip_.size() / density_ratio);
    }

    uint32_t operator () (uint32_t /* s */) {
        size_t strip_size = strip_.size();
        size_t strip_parts = 1;

        while (pixis_count_ < pixis_.size())
        {
            // make new pixi
            size_t j;
            for (j = 0; j < pixis_.size(); ++j) {
                if (pixis_[j].on == false)
                    break;
            }
            if (j < pixis_.size()) {
                Pixi& p = pixis_[j];

                p.on = true;
                p.part = random(strip_parts);
                p.pos = random(strip_size);
                p.size = 1.0 + random(20000) / 1000.0;
                p.speed = norm_(rng_) * p.size;
                p.ts = 0;
                p.ts_end = 1 + random(10);
                p.hue = random(HSV_HUE_MAX);
                ++pixis_count_;
            }
        }

        for (size_t i = 0; i < strip_size; ++i) {
            strip_.setPixel(i, 0);
        }
        uint8_t intensity = strip_.intensity();
        for (size_t i = 0; i < pixis_.size(); ++i) {
            if (!pixis_[i].on)
                continue;

            Pixi& p = pixis_[i];

            for (size_t k = 0; k < p.size; ++k) {
                Color color = HSVColor(
                    p.hue, 255, intensity
                    * sin(p.ts * M_PI / p.ts_end)
                    * sin(k * M_PI / p.size));
                strip_.orPixel(p.part * strip_size + p.pos + k - p.size / 2.0,
                               color);
            }

            p.ts++;
            if (p.ts >= p.ts_end) {
                p.on = false;
                --pixis_count_;
            }
        }

        return 10;
    }
};

template <typename LEDStrip>
class Starlight
{
public:
    Starlight(LEDStrip& strip) : strip_(strip) {
        strip_size_ = strip.size();

        for (size_t i = 0; i < max_blinks_; ++i) {
            blinks_[i] = strip_size_;
        }
    }

    LEDStrip& strip_;

    uint32_t operator () (uint32_t s) {
        unsigned intensity = strip_.intensity();

        // Color w = Color(32, 0, 0, 100);
        Color w = Color(0, 0, 0, intensity);
        // Color w = Color(0, 0, 0, 2);

        for (size_t i = 0; i < strip_size_; ++i)
            strip_.setPixel(i, w);

        if (rng_() % 128 < 16 && 0) {
            // add new blink
            for (size_t i = 0; i < max_blinks_; ++i) {
                if (blinks_[i] >= strip_size_) {
                    blinks_[i] = rng_() % strip_size_;
                    blinks_age_[i] = 0;
                    break;
                }
            }
        }

        for (size_t i = 0; i < max_blinks_; ++i) {
            if (blinks_[i] >= strip_size_)
                continue;

            if (dist_(rng_) * 100 < blinks_age_[i]) {
                size_t v = ldist_(rng_) * 10;
                strip_.setPixel(blinks_[i], v);
            }

            blinks_age_[i]++;
            if (blinks_age_[i] > 200)
                blinks_[i] = strip_size_;
        }

        return 50000;
    }

protected:
    static const size_t max_blinks_ = 8;
    size_t strip_size_;

    std::default_random_engine rng_ { 12345 };
    std::normal_distribution<float> dist_ { 1.0, 1.0 };
    std::lognormal_distribution<float> ldist_ { 0.0, 0.5 };

    size_t blinks_[max_blinks_];
    size_t blinks_age_[max_blinks_];
    size_t blinks_hue_[max_blinks_];
};

template <typename LEDStrip>
class CountPattern
{
public:
    CountPattern(LEDStrip& strip) : strip_(strip) { }

    LEDStrip& strip_;

    uint32_t operator () (uint32_t /* s */) {
        size_t strip_size = strip_.size();
        unsigned intensity = strip_.intensity();

        for (size_t i = 0; i < strip_size; ++i) {
            strip_.setPixel(i, 0);
        }

        Color colors[] = {
            Color(intensity, 0, 0),
            Color(0, intensity, 0),
            Color(0, 0, intensity),
            Color(intensity, 0, intensity),
            Color(0, intensity, intensity),
        };

        for (size_t i = 0; i < strip_size / 10; ++i) {
            strip_.setPixel(i * 10 + 0, Color(intensity));
        }
        for (size_t i = 0; i < strip_size / 10; ++i) {
            strip_.setPixel(i * 10 + 1, colors[i % 5]);
        }
        for (size_t i = 0; i < strip_size / 10; ++i) {
            strip_.setPixel(i * 10 + 2, colors[(i / 10) % 5]);
        }

        return 20000;
    }
};

/******************************************************************************/

template <typename LEDStrip>
void RunRandomFluxAnimations(LEDStrip& strip) {
    static const size_t time_limit = 20000;

    while (1) {
        for (size_t i = 0; i < strip.size(); ++i)
            strip.setPixel(i, 0);

        size_t a = random(15);
        // a = 15;
        switch (a) {
        case 0:
            RunAnimation(
                ColorWipeRGBW<LEDStrip>(strip),
                time_limit);
            break;
        case 1:
            RunAnimation(
                ColorWipeTwoSine<LEDStrip>(strip),
                time_limit);
            break;
        case 2:
            RunAnimation(
                WheelColorWheel<LEDStrip>(strip),
                time_limit);
            break;
        case 3:
            RunAnimation(
                HSVColorWheel<LEDStrip>(strip),
                time_limit);
            break;

        case 4:
            RunAnimation(
                SparkleWhite<LEDStrip>(strip),
                time_limit);
            break;
        case 5:
            RunAnimation(
                SparkleWhite<LEDStrip>(strip, /* speed */ 2000, /* density */ 5),
                time_limit);
            break;
        case 6:
            RunAnimation(
                SparkleRGB<LEDStrip>(strip),
                time_limit);
            break;
        case 7:
            RunAnimation(
                SparkleRGB<LEDStrip>(strip, /* speed */ 2000, /* density */ 5),
                time_limit);
            break;

        case 8:
            RunAnimation(
                Fire<LEDStrip>(strip),
                time_limit);
            break;
        case 9:
            RunAnimation(
                FireIce<LEDStrip>(strip),
                time_limit);
            break;

        case 10:
            RunAnimation(
                SprayColor<LEDStrip>(strip, /* reverse */ false),
                time_limit);
            break;
        case 11:
            RunAnimation(
                SprayColor<LEDStrip>(strip, /* reverse */ true),
                time_limit);
            break;

        case 12:
            RunAnimation(
                Fireworks<LEDStrip>(strip),
                time_limit);
            break;
        case 13:
            RunAnimation(
                KnightSnakes<LEDStrip, /* TrueHSV */ true>(strip, 25000, 40),
                time_limit);
            break;
        case 14:
            RunAnimation(
                PulseColor<LEDStrip>(strip),
                time_limit);
            break;
            // case 15:
            //     RunAnimation(
            //         Starlight<LEDStrip>(strip),
            //         time_limit);
            //     break;
        }
    }
}

/******************************************************************************/

} // namespace BlinkenAlgorithms

#endif // !BLINKENALGORITHMS_ANIMATION_FLUX_HEADER

/******************************************************************************/
