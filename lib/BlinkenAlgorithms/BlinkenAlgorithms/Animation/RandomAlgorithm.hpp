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

    static size_t a = 16;
    //size_t a = random(22);
    //a = 4;
    switch (a) {
    case 0:
        RunSort(strip, "Selection Sort", SelectionSort, 400); // 20 secs
        break;
    case 1:
        RunSort(strip, "Insertion Sort", InsertionSort, 250); // 17 secs
        break;
    case 2:
        RunSort(strip, "Bubble Sort", BubbleSort, 210); // 20 secs
        break;
    case 3:
        RunSort(strip, "Cocktail-Shaker Sort", CocktailShakerSort, 250); // 20 secs
        break;
    case 4:
        RunSort(strip, "QuickSort (LR)\nHoare", QuickSortLR, 2000); // 10 secs
        break;
    case 5:
        RunSort(strip, "QuickSort (LL)\nLomoto", QuickSortLL, 1700); // 10 secs
        break;
    case 6:
        RunSort(strip, "QuickSort\nDual Pivot", QuickSortDualPivot, 2000); // 9 secs
        break;
    case 7:
        RunSort(strip, "MergeSort", MergeSort, 4000); // 10 secs
        break;
    case 8:
        RunSort(strip, "ShellSort", ShellSort, 2500); // 15 secs
        break;
    case 9:
        RunSort(strip, "HeapSort", HeapSort, 1800); // 15 secs
        break;
    case 10:
        RunSort(strip, "CycleSort", CycleSort, 130); // 20 secs
        break;
    case 11:
        RunSort(strip, "RadixSortMSD", RadixSortMSD, 2000); // 13 secs
        break;
    case 12:
        RunSort(strip, "RadixSortLSD", RadixSortLSD, 8500); // 13 secs
        break;
    case 13:
        RunSort(strip, "std::sort", StdSort, 2100); // 10 secs
        break;
    case 14:
        RunSort(strip, "std::stable_sort", StdStableSort, 3200); // 12.2 secs
        break;
    case 15:
        RunSort(strip, "WikiSort", WikiSort, 1600); // 18 secs
        break;
    case 16:
        RunSort(strip, "TimSort", TimSort, 2400); // 12 secs
        break;
    case 17:
        RunSort(strip, "BozoSort", BozoSort); // 20 secs (break time)
        break;

#define TRunHash(A, T) \
    RunHash(strip, #A, A, T)

    case 18:
        TRunHash(LinearProbingHT, 3000); // 10.8 secs
        break;
    case 19:
        TRunHash(QuadraticProbingHT, 5000); // 10.8 secs
        break;
    case 20:
        TRunHash(CuckooHashingTwo, 12000); // 10 secs
        break;
    case 21:
        TRunHash(CuckooHashingThree, 4500); // 11 secs
        break;
#if 0

        // case 22:
        //     RunLawaSAT(strip);
        //     break;
#endif
    }
#undef TRunSort
    ++a;
    a %= 22;
}

} // namespace BlinkenAlgorithms

#endif // !BLINKENALGORITHMS_ANIMATION_RANDOMALGORITHM_HEADER

/******************************************************************************/
