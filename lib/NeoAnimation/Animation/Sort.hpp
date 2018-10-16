/*******************************************************************************
 * lib/NeoAnimation/Animation/Sort.hpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#ifndef NEOANIMATION_ANIMATION_SORT_HEADER
#define NEOANIMATION_ANIMATION_SORT_HEADER

#include <NeoAnimation/Color.hpp>
#include <NeoAnimation/Control.hpp>

#include <cassert>
#include <random>
#include <vector>

#include "TimSort.hpp"
#include "WikiSort.hpp"

namespace NeoSort {

using namespace NeoAnimation;

static const uint16_t black = uint16_t(-1);
static const uint16_t unsigned_negative = uint16_t(32678);

/******************************************************************************/
//! custom struct for array items, which allows detailed counting of comparisons

class Item
{
public:
    typedef uint16_t value_type;

public:
    value_type value_;

public:
    Item() { }

    explicit Item(const value_type& d) : value_(d) { OnChange(this); }

    Item(const Item& v) : value_(v.value_) { OnChange(this); }

    Item& operator = (const Item& a) {
        value_ = a.value_;
        OnChange(this);
        return *this;
    }

    static void OnChange(const Item* a, bool with_delay = true);

    //! Returns value_
    value_type value() const {
        OnChange(this, /* with_delay */ true);
        return value_;
    }

    Item& operator ++ (int) {
        value_++;
        OnChange(this);
        return *this;
    }

    Item& operator -- (int) {
        value_--;
        OnChange(this);
        return *this;
    }

    // *** bypass delay and updates

    Item& SetNoDelay(const value_type& d) {
        value_ = d;
        OnChange(this, /* with_delay */ false);
        return *this;
    }

    Item& SetNoDelay(const Item& a) {
        value_ = a.value_;
        OnChange(this, /* with_delay */ false);
        return *this;
    }

    void SwapNoDelay(Item& a) {
        Item tmp;
        tmp.SetNoDelay(a);
        a.SetNoDelay(*this);
        SetNoDelay(tmp);
    }

    // *** comparisons

    bool operator == (const Item& v) const {
        OnComparison(*this, v);
        return (value_ == v.value_);
    }

    bool operator != (const Item& v) const {
        OnComparison(*this, v);
        return (value_ != v.value_);
    }

    bool operator < (const Item& v) const {
        OnComparison(*this, v);
        return (value_ < v.value_);
    }

    bool operator <= (const Item& v) const {
        OnComparison(*this, v);
        return (value_ <= v.value_);
    }

    bool operator > (const Item& v) const {
        OnComparison(*this, v);
        return (value_ > v.value_);
    }

    bool operator >= (const Item& v) const {
        OnComparison(*this, v);
        return (value_ >= v.value_);
    }

    // ternary comparison which counts just one
    int cmp(const Item& v) const {
        OnComparison(*this, v);
        return (value_ == v.value_ ? 0 : value_ < v.value_ ? -1 : +1);
    }

    // *** comparisons without sound, counting or delay

    bool equal_direct(const Item& v) const { return (value_ == v.value_); }

    bool less_direct(const Item& v) const { return (value_ < v.value_); }

    bool greater_direct(const Item& v) const { return (value_ > v.value_); }

    static void OnComparison(const Item&, const Item&);
};

class SortAnimationBase
{
public:
    virtual void OnChange(const Item* a, bool with_delay) = 0;
};

static SortAnimationBase* sort_animation_hook = nullptr;

static void (* SoundAccess)(size_t i) = nullptr;

void Item::OnChange(const Item* a, bool with_delay) {
    if (sort_animation_hook)
        sort_animation_hook->OnChange(a, with_delay);
    if (SoundAccess)
        SoundAccess(a->value_);
}

void Item::OnComparison(const Item& a, const Item& b) {
    OnChange(&a);
    OnChange(&b);
    if (SoundAccess) {
        SoundAccess(a.value_);
        SoundAccess(b.value_);
    }
}

/******************************************************************************/
// Sorting Algorithms

size_t array_size;
std::vector<Item> A;

using std::swap;

/******************************************************************************/
// Selection Sort

void SelectionSort() {
    for (size_t i = 0; i < array_size - 1; ++i) {
        size_t j_min = i;

        for (size_t j = i + 1; j < array_size; ++j) {
            if (A[j] < A[j_min]) {
                j_min = j;
            }
        }

        swap(A[i], A[j_min]);
    }
}

/******************************************************************************/
// Insertion Sort

void InsertionSort() {
    for (size_t i = 1; i < array_size; ++i) {
        Item key = A[i];

        ssize_t j = i - 1;
        while (j >= 0 && A[j] > key) {
            swap(A[j], A[j + 1]);
            j--;
        }
    }
}

/******************************************************************************/
// Bubble Sort

void BubbleSort() {
    for (size_t i = 0; i < array_size - 1; ++i) {
        for (size_t j = 0; j < array_size - 1 - i; ++j) {
            if (A[j] > A[j + 1]) {
                swap(A[j], A[j + 1]);
            }
        }
    }
}

/******************************************************************************/
// Cocktail Shaker Sort

void CocktailShakerSort() {
    size_t lo = 0, hi = array_size - 1, mov = lo;

    while (lo < hi) {
        for (size_t i = hi; i > lo; --i) {
            if (A[i - 1] > A[i]) {
                swap(A[i - 1], A[i]);
                mov = i;
            }
        }

        lo = mov;

        for (size_t i = lo; i < hi; ++i) {
            if (A[i] > A[i + 1]) {
                swap(A[i], A[i + 1]);
                mov = i;
            }
        }

        hi = mov;
    }
}

/******************************************************************************/
// QuickSort

enum QuickSortPivotType {
    PIVOT_FIRST,   //!< always select first item
    PIVOT_LAST,    //!< always select last item
    PIVOT_MID,     //!< always select middle item
    PIVOT_RANDOM,  //!< always pick a random item
    PIVOT_MEDIAN3, //!< select median of three
    PIVOT_SIZE
};

QuickSortPivotType g_quicksort_pivot = PIVOT_FIRST;

// pivot selection method
ssize_t QuickSortSelectPivot(ssize_t lo, ssize_t hi) {
    if (g_quicksort_pivot == PIVOT_FIRST)
        return lo;

    if (g_quicksort_pivot == PIVOT_LAST)
        return hi - 1;

    if (g_quicksort_pivot == PIVOT_MID)
        return (lo + hi) / 2;

    if (g_quicksort_pivot == PIVOT_RANDOM)
        return lo + (rand() % (hi - lo));

    if (g_quicksort_pivot == PIVOT_MEDIAN3) {
        ssize_t mid = (lo + hi) / 2;

        // cases if two are equal
        if (A[lo] == A[mid])
            return lo;
        if (A[lo] == A[hi - 1] || A[mid] == A[hi - 1])
            return hi - 1;

        // cases if three are different
        return A[lo] < A[mid]
               ? (A[mid] < A[hi - 1] ? mid
                  : (A[lo] < A[hi - 1] ? hi - 1 : lo))
               : (A[mid] > A[hi - 1] ? mid
                  : (A[lo] < A[hi - 1] ? lo : hi - 1));
    }

    return lo;
}

/******************************************************************************/
// Quick Sort LR (pointers left and right, Hoare's partition schema)

void QuickSortLR(ssize_t lo, ssize_t hi) {
    ssize_t p = QuickSortSelectPivot(lo, hi + 1);

    ssize_t i = lo, j = hi;

    while (i <= j) {
        while (A[i] < A[p])
            i++;
        while (A[j] > A[p])
            j--;
        if (i <= j) {
            swap(A[i], A[j]);

            // follow pivot if it is swapped
            if (p == i)
                p = j;
            else if (p == j)
                p = i;

            i++, j--;
        }
    }

    if (lo < j)
        QuickSortLR(lo, j);
    if (i < hi)
        QuickSortLR(i, hi);
}

void QuickSortLR() {
    g_quicksort_pivot = (QuickSortPivotType)random(PIVOT_SIZE);
    QuickSortLR(0, array_size - 1);
}

/******************************************************************************/
// Quick Sort LL (Lomuto partition scheme, two pointers at left, pivot is moved
// to the right) (code by Timo Bingmann, based on CLRS' 3rd edition)

size_t PartitionLL(size_t lo, size_t hi) {
    // pick pivot and move to back
    size_t p = QuickSortSelectPivot(lo, hi);

    Item& pivot = A[p];
    swap(A[p], A[hi - 1]);

    ssize_t i = lo;

    for (size_t j = lo; j < hi - 1; ++j) {
        if (A[j] <= pivot) {
            swap(A[i], A[j]);
            ++i;
        }
    }

    swap(A[i], A[hi - 1]);

    return i;
}

void QuickSortLL(size_t lo, size_t hi) {
    if (lo + 1 < hi) {
        size_t mid = PartitionLL(lo, hi);

        QuickSortLL(lo, mid);
        QuickSortLL(mid + 1, hi);
    }
}

void QuickSortLL() {
    g_quicksort_pivot = (QuickSortPivotType)random(PIVOT_SIZE);
    QuickSortLL(0, array_size);
}

/******************************************************************************/
// Dual-Pivot Quick Sort (code by Yaroslavskiy via Sebastian Wild)

void QuickSortDualPivotYaroslavskiy(int left, int right) {
    if (right > left) {
        if (A[left] > A[right]) {
            swap(A[left], A[right]);
        }

        const Item p = A[left];
        const Item q = A[right];

        ssize_t l = left + 1;
        ssize_t g = right - 1;
        ssize_t k = l;

        while (k <= g) {
            if (A[k] < p) {
                swap(A[k], A[l]);
                ++l;
            }
            else if (A[k] >= q) {
                while (A[g] > q && k < g)
                    --g;
                swap(A[k], A[g]);
                --g;

                if (A[k] < p) {
                    swap(A[k], A[l]);
                    ++l;
                }
            }
            ++k;
        }
        --l;
        ++g;
        swap(A[left], A[l]);
        swap(A[right], A[g]);

        QuickSortDualPivotYaroslavskiy(left, l - 1);
        QuickSortDualPivotYaroslavskiy(l + 1, g - 1);
        QuickSortDualPivotYaroslavskiy(g + 1, right);
    }
}

void QuickSortDualPivot() {
    return QuickSortDualPivotYaroslavskiy(0, array_size - 1);
}

/******************************************************************************/
// Merge Sort (out-of-place with sentinels) (code by myself, Timo Bingmann)

void Merge(size_t lo, size_t mid, size_t hi) {
    // allocate output
    Item out[hi - lo];

    // merge
    size_t i = lo, j = mid, o = 0; // first and second halves
    while (i < mid && j < hi) {
        // copy out for fewer time steps
        Item ai = A[i], aj = A[j];

        out[o++] = (ai < aj ? (++i, ai) : (++j, aj));
    }

    // copy rest
    while (i < mid)
        out[o++] = A[i++];
    while (j < hi)
        out[o++] = A[j++];

    // copy back
    for (i = 0; i < hi - lo; ++i)
        A[lo + i] = out[i];
}

void MergeSort(size_t lo, size_t hi) {
    if (lo + 1 < hi) {
        size_t mid = (lo + hi) / 2;

        MergeSort(lo, mid);
        MergeSort(mid, hi);

        Merge(lo, mid, hi);
    }
}

void MergeSort() {
    return MergeSort(0, array_size);
}

void MergeSortIterative() {
    for (size_t s = 1; s < array_size; s *= 2) {
        for (size_t i = 0; i + s < array_size; i += 2 * s) {
            Merge(i, i + s, std::min(i + 2 * s, array_size));
        }
    }
}

/******************************************************************************/
// Shell's Sort

void ShellSort() {
    size_t incs[16] = {
        1391376, 463792, 198768, 86961, 33936, 13776, 4592, 1968,
        861, 336, 112, 48, 21, 7, 3, 1
    };

    for (size_t k = 0; k < 16; k++) {
        for (size_t h = incs[k], i = h; i < array_size; i++) {
            Item v = A[i];
            size_t j = i;

            while (j >= h && A[j - h] > v) {
                A[j] = A[j - h];
                j -= h;
            }

            A[j] = v;
        }
    }
}

/******************************************************************************/
// Heap Sort

bool isPowerOfTwo(size_t x) {
    return ((x != 0) && !(x & (x - 1)));
}

uint32_t prevPowerOfTwo(uint32_t x) {
    x |= x >> 1, x |= x >> 2, x |= x >> 4, x |= x >> 8, x |= x >> 16;
    return x - (x >> 1);
}

int largestPowerOfTwoLessThan(int n) {
    int k = 1;
    while (k < n)
        k = k << 1;
    return k >> 1;
}

void HeapSort() {
    size_t n = array_size, i = n / 2;

    while (1) {
        if (i > 0) {
            // build heap, sift A[i] down the heap
            i--;
        }
        else {
            // pop largest element from heap: swap front to back, and sift
            // front A[0] down the heap
            n--;
            if (n == 0)
                return;
            swap(A[0], A[n]);
        }

        size_t parent = i;
        size_t child = i * 2 + 1;

        // sift operation - push the value_ of A[i] down the heap
        while (child < n) {
            if (child + 1 < n && A[child + 1] > A[child]) {
                child++;
            }
            if (A[child] > A[parent]) {
                swap(A[parent], A[child]);
                parent = child;
                child = parent * 2 + 1;
            }
            else {
                break;
            }
        }
    }
}

/******************************************************************************/
// Cycle Sort (adapted from http://en.wikipedia.org/wiki/Cycle_sort)

void CycleSort(ssize_t n) {
    ssize_t cycleStart = 0;
    ssize_t rank = 0;

    // Loop through the array to find cycles to rotate.
    for (cycleStart = 0; cycleStart < n - 1; ++cycleStart) {
        Item& item = A[cycleStart];

        do {
            // Find where to put the item.
            rank = cycleStart;
            for (ssize_t i = cycleStart + 1; i < n; ++i) {
                if (A[i] < item)
                    rank++;
            }

            // If the item is already there, this is a 1-cycle.
            if (rank == cycleStart) {
                break;
            }

            // Otherwise, put the item after any duplicates.
            while (item == A[rank])
                rank++;

            // Put item into right place and colorize
            swap(A[rank], A[cycleStart]);

            // Continue for rest of the cycle.
        } while (rank != cycleStart);
    }
}

void CycleSort() {
    CycleSort(array_size);
}

/******************************************************************************/
// Radix Sort (counting sort, most significant digit (MSD) first, in-place
// redistribute) (code by myself, Timo Bingmann)

void RadixSortMSD(size_t lo, size_t hi, size_t depth) {
    // radix and base calculations
    const unsigned int RADIX = 4;

    unsigned int pmax = floor(log(array_size) / log(RADIX));
    size_t base = pow(RADIX, pmax - depth);

    // count digits
    std::vector<size_t> count(RADIX, 0);

    for (size_t i = lo; i < hi; ++i) {
        size_t r = A[i].value() / base % RADIX;
        count[r]++;
    }

    // inclusive prefix sum
    std::vector<size_t> bkt(RADIX, 0);
    std::partial_sum(count.begin(), count.end(), bkt.begin());

    // reorder items in-place by walking cycles
    for (size_t i = 0, j; i < (hi - lo); ) {
        while ((j = --bkt[(A[lo + i].value() / base % RADIX)]) > i) {
            swap(A[lo + i], A[lo + j]);
        }
        i += count[(A[lo + i].value() / base % RADIX)];
    }

    // no more depth to sort?
    if (depth + 1 > pmax)
        return;

    // recurse on buckets
    size_t sum = lo;
    for (size_t i = 0; i < RADIX; ++i) {
        if (count[i] > 1)
            RadixSortMSD(sum, sum + count[i], depth + 1);
        sum += count[i];
    }
}

void RadixSortMSD() {
    return RadixSortMSD(0, array_size, 0);
}

/******************************************************************************/
// Radix Sort (counting sort, least significant digit (LSD) first, out-of-place
// redistribute) (code by myself, Timo Bingmann)

void RadixSortLSD() {
    // radix and base calculations
    const unsigned int RADIX = 4;

    unsigned int pmax = ceil(log(array_size) / log(RADIX));

    for (unsigned int p = 0; p < pmax; ++p) {
        size_t base = pow(RADIX, p);

        // count digits and copy data
        std::vector<size_t> count(RADIX, 0);
        std::vector<Item> copy(array_size);

        for (size_t i = 0; i < array_size; ++i) {
            size_t r = (copy[i] = A[i]).value() / base % RADIX;
            assert(r < RADIX);
            count[r]++;
        }

        // exclusive prefix sum
        std::vector<size_t> bkt(RADIX + 1, 0);
        std::partial_sum(count.begin(), count.end(), bkt.begin() + 1);

        // redistribute items back into array (stable)
        for (size_t i = 0; i < array_size; ++i) {
            size_t r = copy[i].value() / base % RADIX;
            A[bkt[r]++] = copy[i];
        }
    }
}

/******************************************************************************/

void StdSort() {
    std::sort(A.begin(), A.end());
}

void StdStableSort() {
    std::stable_sort(A.begin(), A.end());
}

/******************************************************************************/

void WikiSort() {
    WikiSortNS::Sort(A.begin(), A.end(), std::less<Item>());
}

/******************************************************************************/

void TimSort() {
    TimSortNS::timsort(A.begin(), A.end());
}

/******************************************************************************/
// BozoSort

void BozoSort() {
    unsigned long ts = millis() + 20000;
    while (millis() < ts) {
        // swap two random items
        swap(A[random(array_size)], A[random(array_size)]);
        // swap two random items
        swap(A[random(array_size)], A[random(array_size)]);
        // swap two random items
        swap(A[random(array_size)], A[random(array_size)]);
        // swap two random items
        swap(A[random(array_size)], A[random(array_size)]);
    }
}

/******************************************************************************/

template <typename LEDStrip>
class SortAnimation : public SortAnimationBase
{
public:
    SortAnimation(LEDStrip& strip, int32_t delay_time = 1000)
        : strip_(strip), delay_time_(delay_time) {
        // hook sorting animation callbacks
        sort_animation_hook = this;

        // set strip size
        array_size = strip_.size();
        A.resize(array_size);

        if (delay_time_ < 0)
            frame_drop = -delay_time_;
    }

    void array_randomize() {
        for (uint32_t i = 0; i < array_size; ++i) {
            A[i].SetNoDelay(i);
        }
        for (uint32_t i = 0; i < array_size; ++i) {
            uint32_t j = random(array_size);
            A[i].SwapNoDelay(A[j]);
        }
    }

    void array_black() {
        for (uint32_t i = 0; i < array_size; ++i) {
            A[i].SetNoDelay(black);
        }
    }

    void array_check() {
        for (size_t i = 1; i < array_size; ++i) {
            if (A[i - 1] >= A[i]) {
                A[i - 1] = Item(black);
            }
        }
    }

    unsigned intensity_high = 255;
    unsigned intensity_low = 64;

    void OnChange(const Item* a, bool with_delay) override {
        if (a < A.data() || a >= A.data() + array_size)
            return;
        flash(a - A.data(), with_delay);
    }

    void yield_delay() {
        if (delay_time_ >= 0)
            delay_micros(delay_time_);
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

    size_t frame_buffer[128] = { 0 };
    size_t frame_buffer_pos = 0;
    size_t frame_drop = 0;

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
        yield_delay();

        strip_.show();
    }

protected:
    LEDStrip& strip_;

    //! user given delay time.
    int32_t delay_time_;
};

template <typename LEDStrip>
void RunSort(LEDStrip& strip, void (* sort_function)(), int32_t delay_time = 10000) {
    uint32_t ts = millis();
    SortAnimation<LEDStrip> ani(strip, delay_time);
    ani.array_randomize();
    sort_function();
    std::cout << "Running time: " << (millis() - ts) / 1000.0 << std::endl;
    ani.array_check();
    ani.pflush();
}

/******************************************************************************/

template <typename LEDStrip>
void RunAllSortAnimation(LEDStrip& strip) {
    RunSort(strip, SelectionSort);
    RunSort(strip, InsertionSort);
    RunSort(strip, BubbleSort);
    RunSort(strip, CocktailShakerSort);
    RunSort(strip, QuickSortLR);
    RunSort(strip, QuickSortDualPivot);
    RunSort(strip, MergeSort);
    RunSort(strip, ShellSort);
    RunSort(strip, HeapSort);
    RunSort(strip, CycleSort);
    RunSort(strip, RadixSortMSD);
    RunSort(strip, RadixSortLSD);
    RunSort(strip, StdSort);
    RunSort(strip, StdStableSort);
    RunSort(strip, WikiSort);
    RunSort(strip, TimSort);
    RunSort(strip, BozoSort);
}

} // namespace NeoSort

#endif // !NEOANIMATION_ANIMATION_SORT_HEADER

/******************************************************************************/
