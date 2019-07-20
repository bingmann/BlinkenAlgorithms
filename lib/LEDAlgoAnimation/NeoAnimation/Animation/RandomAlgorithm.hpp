/*******************************************************************************
 * lib/NeoAnimation/Animation/RandomAlgorithm.hpp
 *
 * Copyright (C) 2018 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#ifndef NEOANIMATION_ANIMATION_RANDOMALGORITHM_HEADER
#define NEOANIMATION_ANIMATION_RANDOMALGORITHM_HEADER

#include <NeoAnimation/Animation/HashTable.hpp>
#include <NeoAnimation/Animation/LawaSAT.hpp>
#include <NeoAnimation/Animation/Sort.hpp>

namespace NeoAnimation {

template <typename LEDStrip>
void RunRandomAlgorithmAnimation(LEDStrip& strip) {
    static float r = 1.0, s = 0.5;

    using namespace NeoSort;
    using namespace NeoHashTable;
    using namespace NeoLawaSAT;

    uint32_t ts = millis();

    size_t a = random(23);
    switch (a) {
    // ---[ Sorting ]-----------------------------------------------------------
    case 0:
        RunSort(strip, SelectionSort, -42);
        break;
    case 1:
        RunSort(strip, InsertionSort, -21);
        break;
    case 2:
        RunSort(strip, BubbleSort, -62);
        break;
    case 3:
        RunSort(strip, CocktailShakerSort, -52);
        break;
    case 4:
        // RunSort(strip, QuickSortLR, r * 500);
        RunSort(strip, QuickSortLR, -21);
        break;
    case 5:
        RunSort(strip, QuickSortLL, r * 500);
        break;
    case 6:
        RunSort(strip, QuickSortDualPivot, 2000);
        break;
    case 7:
        RunSort(strip, MergeSort, 8000);
        break;
    case 8:
        RunSort(strip, ShellSort, 800);
        break;
    case 9:
        RunSort(strip, HeapSort, 100);
        break;
    case 10:
        RunSort(strip, CycleSort, -30);
        break;
    case 11:
        RunSort(strip, RadixSortMSD, 1000);
        break;
    case 12:
        RunSort(strip, RadixSortLSD, 18000);
        break;
    case 13:
        RunSort(strip, StdSort, 1000);
        break;
    case 14:
        RunSort(strip, StdStableSort, 2000);
        break;
    case 15:
        RunSort(strip, WikiSort, 50);
        break;
    case 16:
        RunSort(strip, TimSort, 2000);
        break;
    case 17:
        RunSort(strip, BozoSort);
        break;

    // ---[ Hashing ]-----------------------------------------------------------
    case 18:
        RunHash(strip, LinearProbingHT, 1200);
        break;
    case 19:
        RunHash(strip, QuadraticProbingHT, 5000);
        break;
    case 20:
        RunHash(strip, CuckooHashingTwo, 5000);
        break;
    case 21:
        RunHash(strip, CuckooHashingThree, 5000);
        break;

    // ---[ Other ]-------------------------------------------------------------
    case 22:
        RunLawaSAT(strip);
        break;
    }

    double t = (millis() - ts) / 1000.0;

    if (t > 60.0) {
        r = r - s, s /= 2.0;
    }
    else {
        r = r + s, s *= 2.0;
    }
}

} // namespace NeoAnimation

#endif // !NEOANIMATION_ANIMATION_RANDOMALGORITHM_HEADER

/******************************************************************************/
