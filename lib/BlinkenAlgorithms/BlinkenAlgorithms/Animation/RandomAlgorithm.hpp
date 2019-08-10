/*******************************************************************************
 * lib/BlinkenAlgorithms/BlinkenAlgorithms/Animation/RandomAlgorithm.hpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#ifndef BLINKENALGORITHMS_ANIMATION_RANDOMALGORITHM_HEADER
#define BLINKENALGORITHMS_ANIMATION_RANDOMALGORITHM_HEADER

#include <BlinkenAlgorithms/Animation/Hashtable.hpp>
#include <BlinkenAlgorithms/Animation/LawaSAT.hpp>
#include <BlinkenAlgorithms/Animation/Sort.hpp>

namespace BlinkenAlgorithms {

/*!
 * BlinkenAlgorithms with delay time adapted for ESP8266 and Teensy 3.6 such
 * that most algorithms run approximately 15 seconds each for a strip with 300
 * LEDs.  Asymptotically slower algorithms run 20 seconds, faster ones 10 or
 * less.
 */
template <typename LEDStrip>
void RunRandomAlgorithmAnimation(LEDStrip& strip) {
    for (size_t i = 0; i < strip.size(); ++i)
        strip.setPixel(i, 0);

    using namespace BlinkenSort;
    using namespace BlinkenHashtable;
    using namespace BlinkenLawaSAT;

    static size_t a = 0;
    // size_t a = random(22);
    // a = 20;
    switch (a) {
    case 0:
        RunSort(strip, "MergeSort", MergeSort, 9500); // 24 secs
        break;
    case 1:
        RunSort(strip, "Insertion Sort", InsertionSort, 750); // 50 secs
        break;
    case 2:
        RunSort(strip, "QuickSort (LR)\nHoare", QuickSortLR, 5000); // 27 secs
        break;
    case 3:
        RunSort(strip, "QuickSort (LL)\nLomoto", QuickSortLL, 4600); // 27 secs
        break;
    case 4:
        RunSort(strip, "QuickSort\nDual Pivot", QuickSortDualPivot, 5200); // 24 secs
        break;
    case 5:
        RunSort(strip, "ShellSort", ShellSort, 6500); // 42 secs
        break;
    case 6:
        RunSort(strip, "HeapSort", HeapSort, 4800); // 42 secs
        break;
    case 7:
        RunSort(strip, "CycleSort", CycleSort, 44000); // 40 secs
        break;
    case 8:
        RunSort(strip, "RadixSort-MSD\n(High First)", RadixSortMSD, 4200); // 27 secs
        break;
    case 9:
        RunSort(strip, "RadixSort-LSD\n(Low First)", RadixSortLSD, 17500); // 27 secs
        break;
    case 10:
        RunSort(strip, "std::sort", StdSort, 4500); // 22 secs
        break;
    case 11:
        RunSort(strip, "std::stable_sort", StdStableSort, 7300); // 28 secs
        break;
    case 12:
        RunSort(strip, "WikiSort", WikiSort, 3900); // 42 secs
        break;
    case 13:
        RunSort(strip, "TimSort", TimSort, 5300); // 27 secs
        break;
    case 14:
        RunSort(strip, "Selection Sort", SelectionSort, 1250); // 60 secs
        break;
    case 15:
        RunSort(strip, "Bubble Sort", BubbleSort, 650); // 60 secs
        break;
    case 16:
        RunSort(strip, "Cocktail-Shaker Sort", CocktailShakerSort, 770); // 58 secs
        break;
    case 17:
        RunSort(strip, "BozoSort", BozoSort); // 20 secs (break time)
        break;

    /*------------------------------------------------------------------------*/

    case 18:
        RunHash(strip, "Linear Probe\nHash Table", // 41 secs
                LinearProbingHT, 40000);
        break;
    case 19:
        RunHash(strip, "Quadratic Probe Hash Table", // 35 secs
                QuadraticProbingHT, 27000);
        break;
    case 20:
        RunHash(strip, "Cuckoo Two\nHash Table", // 35 secs
                CuckooHashingTwo, 40000);
        break;
    case 21:
        RunHash(strip, "Cuckoo Three\nHash Table", // 35 secs
                CuckooHashingThree, 35000);
        break;

    /*------------------------------------------------------------------------*/

    case 22:
        // RunLawaSAT(strip);
        break;
    }
    ++a;
    a %= 22;
}

} // namespace BlinkenAlgorithms

#endif // !BLINKENALGORITHMS_ANIMATION_RANDOMALGORITHM_HEADER

/******************************************************************************/
