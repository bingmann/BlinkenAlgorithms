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
    while (1) {
        for (size_t i = 0; i < strip.size(); ++i)
            strip.setPixel(i, 0);

        using namespace NeoSort;
        using namespace NeoHashTable;
        using namespace NeoLawaSAT;

        size_t a = random(22);
        switch (a) {
        case 0:
            RunSort(strip, SelectionSort, -10);
            break;
        case 1:
            RunSort(strip, InsertionSort, -3);
            break;
        case 2:
            RunSort(strip, BubbleSort, -8);
            break;
        case 3:
            RunSort(strip, CocktailShakerSort, -8);
            break;
        case 4:
            RunSort(strip, QuickSortLR);
            break;
        case 5:
            RunSort(strip, QuickSortDualPivot);
            break;
        case 6:
            RunSort(strip, MergeSort);
            break;
        case 7:
            RunSort(strip, ShellSort);
            break;
        case 8:
            RunSort(strip, HeapSort);
            break;
        case 9:
            RunSort(strip, CycleSort);
            break;
        case 10:
            RunSort(strip, RadixSortMSD);
            break;
        case 11:
            RunSort(strip, RadixSortLSD);
            break;
        case 12:
            RunSort(strip, StdSort);
            break;
        case 13:
            RunSort(strip, StdStableSort);
            break;
        case 14:
            RunSort(strip, WikiSort);
            break;
        case 15:
            RunSort(strip, TimSort);
            break;
        case 16:
            RunSort(strip, BozoSort);
            break;

        case 17:
            RunHash(strip, LinearProbingHT);
            break;
        case 18:
            RunHash(strip, QuadraticProbingHT);
            break;
        case 19:
            RunHash(strip, CuckooHashingTwo);
            break;
        case 20:
            RunHash(strip, CuckooHashingThree);
            break;

        case 21:
            RunLawaSAT(strip);
            break;
        }
    }
}

} // namespace NeoAnimation

#endif // !NEOANIMATION_ANIMATION_RANDOMALGORITHM_HEADER

/******************************************************************************/
