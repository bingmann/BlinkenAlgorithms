/*******************************************************************************
 * lib/NeoAnimation/Animation/HashTable.hpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#ifndef NEOANIMATION_ANIMATION_HASHTABLE_HEADER
#define NEOANIMATION_ANIMATION_HASHTABLE_HEADER

#include <NeoAnimation/Animation/Sort.hpp>
#include <NeoAnimation/Color.hpp>

#include <cassert>
#include <random>
#include <vector>

namespace NeoHashTable {

using namespace NeoSort;

static const uint16_t black = uint16_t(-1);

template <typename LEDStrip>
class HashAnimation : public SortAnimation<LEDStrip>
{
public:
    using Super = SortAnimation<LEDStrip>;
    using Super::strip_;

    HashAnimation(LEDStrip& strip) : Super(strip) { }

    void array_black() {
        for (uint32_t i = 0; i < array_size; ++i) {
            A[i].SetNoDelay(black);
        }
    }

    unsigned intensity_high = 255;
    unsigned intensity_low = 32;

    void OnChange(const Item* a, bool with_delay) final {
        if (a < A.data() || a >= A.data() + array_size)
            return;
        flash(a - A.data(), with_delay);
    }

    void yield_delay() {
        // delayMicroseconds(100);
        delay_millis(10);
    }

    uint16_t value_to_hue(size_t i) { return i * HSV_HUE_MAX / array_size; }

    void flash_low(size_t i) {
        if (A[i].value_ == black)
            strip_.setPixel(i, 0);
        else
            strip_.setPixel(
                i, HSVColor(value_to_hue(A[i].value_), 255, intensity_low));
    }

    void flash_high(size_t i) {
        if (A[i].value_ == black) {
            strip_.setPixel(i, Color(intensity_high));
        }
        else {
            Color c = HSVColor(value_to_hue(A[i].value_), 255, intensity_high);
            c.white = 128;
            strip_.setPixel(i, c);
        }
    }

    using Super::frame_buffer;
    using Super::frame_buffer_pos;
    using Super::frame_drop;

    void flash(size_t i, bool with_delay = true) {

        if (!with_delay)
            return flash_low(i);

        if (frame_drop == 0) {
            flash_high(i);

            if (!strip_.busy())
                strip_.show();

            yield_delay();

            flash_low(i);
        }
        else {
            flash_high(i);
            frame_buffer[frame_buffer_pos] = i;

            if (frame_buffer_pos == 0) {
                if (!strip_.busy()) {
                    strip_.show();
                }

                // reset pixels in this frame_buffer_pos
                for (size_t j = 0; j < frame_drop; ++j) {
                    if (frame_buffer[j] < array_size)
                        flash_low(frame_buffer[j]);
                }

                frame_buffer_pos = frame_drop - 1;
                yield_delay();
            }
            else {
                --frame_buffer_pos;
            }
        }
    }

    void pflush() {
        // reset pixels in this frame_buffer_pos
        for (size_t j = 0; j < frame_drop; ++j) {
            if (frame_buffer[j] < array_size)
                flash_low(frame_buffer[j]);
        }

        frame_buffer_pos = frame_drop - 1;
        // yield_all();

        // if (delay_time != 0)
        //     yield_micros(delay_time);

        strip_.show();
    }
};

/******************************************************************************/
// Hashing with Linear Probing

uint32_t hash(uint32_t a) {
    a = (a + 0x7ed55d16) + (a << 12);
    a = (a ^ 0xc761c23c) ^ (a >> 19);
    a = (a + 0x165667b1) + (a << 5);
    a = (a + 0xd3a2646c) ^ (a << 9);
    a = (a + 0xfd7046c5) + (a << 3);
    a = (a ^ 0xb55a4f09) ^ (a >> 16);
    return a;
}

void LinearProbingHT() {

    size_t cshift = random(array_size);

    for (size_t i = 0; i < array_size; ++i) {
        // pick a new item to insert
        Item v = Item((i + cshift) % array_size);

        size_t idx = (hash(v.value()) >> 2) % array_size;
        while (A[idx].value() != black) {
            idx = (idx + 1) % array_size;
        }
        A[idx] = v;

        delay_millis(25);
    }
}

/******************************************************************************/
// Hashing with Quadratic Probing

void QuadraticProbingHT() {

    size_t cshift = random(array_size);

    for (size_t i = 0; i < array_size; ++i) {
        // pick a new item to insert
        Item v = Item((i + cshift) % array_size);

        size_t idx = (hash(v.value()) >> 2) % array_size;
        size_t p = 0;
        while (A[idx].value() != black) {
            idx = (idx + (p + p * p) / 2) % array_size;
            ++p;
            if (p == array_size) {
                // cycled. stop hashing.
                return;
            }
        }
        A[idx] = v;
    }
}

/******************************************************************************/
// Cuckoo Hashing with Two Choices

uint32_t hash2(int f, uint32_t a) {
    if (f == 0)
        return (hash(a) >> 2) % array_size;
    if (f == 1)
        return (hash(a) >> 15) % array_size;
    return 0;
}

void CuckooHashingTwo() {

    size_t cshift = random(array_size);

    for (size_t i = 0; i < array_size; ++i) {
        // pick a new item to insert
        Item v = Item((i + cshift) % array_size);

        uint32_t pos = hash2(0, v.value());
        if (A[pos].value() == black) {
            A[pos] = v;
            continue;
        }

        pos = hash2(1, v.value());
        if (A[pos].value() == black) {
            A[pos] = v;
            continue;
        }

        size_t r = 0;
        int hashfunction = 1;
        while (true) {
            pos = hash2(hashfunction, v.value());
            swap(v, A[pos]);
            if (v.value() == black)
                break;

            if (hash2(hashfunction, v.value()) == pos)
                hashfunction = (hashfunction + 1) % 2;

            if (++r >= array_size)
                return;
        }
    }
}

/******************************************************************************/
// Cuckoo Hashing with Three Choices

uint32_t hash3(int f, uint32_t a) {
    if (f == 0)
        return (hash(a) >> 2) % array_size;
    if (f == 1)
        return (hash(a) >> 7) % array_size;
    if (f == 2)
        return (hash(a) >> 15) % array_size;
    return 0;
}

void CuckooHashingThree() {

    size_t cshift = random(array_size);

    for (size_t i = 0; i < array_size; ++i) {
        // pick a new item to insert
        Item v = Item((i + cshift) % array_size);

        uint32_t pos = hash3(0, v.value());
        if (A[pos].value() == black) {
            A[pos] = v;
            continue;
        }

        pos = hash3(1, v.value());
        if (A[pos].value() == black) {
            A[pos] = v;
            continue;
        }

        pos = hash3(2, v.value());
        if (A[pos].value() == black) {
            A[pos] = v;
            continue;
        }

        // all three places full, pick a random one to displace
        int hashfunction = random(3);
        pos = hash3(hashfunction, v.value());
        swap(v, A[pos]);

        size_t r = 0;
        while (true) {
            hashfunction = random(2);
            if (hash3(hashfunction, v.value()) == pos)
                hashfunction = 2;

            pos = hash3(hashfunction, v.value());
            swap(v, A[pos]);
            if (v.value() == black)
                break;

            if (++r >= array_size)
                return;
        }
    }
}

/******************************************************************************/

template <typename LEDStrip>
void RunHash(LEDStrip& strip, void (* hash_function)()) {
    HashAnimation<LEDStrip> ani(strip);
    ani.array_black();
    hash_function();
}

template <typename LEDStrip>
void RunAllHashAnimation(LEDStrip& strip) {
    RunHash(strip, LinearProbingHT);
    RunHash(strip, QuadraticProbingHT);
    RunHash(strip, CuckooHashingTwo);
    RunHash(strip, CuckooHashingThree);
}

} // namespace NeoHashTable

#endif // !NEOANIMATION_ANIMATION_HASHTABLE_HEADER

/******************************************************************************/
