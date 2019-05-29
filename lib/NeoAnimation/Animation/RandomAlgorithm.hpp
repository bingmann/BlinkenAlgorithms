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
    for (size_t i = 0; i < strip.size(); ++i)
        strip.setPixel(i, 0);

    using namespace NeoSort;
    using namespace NeoHashTable;
    using namespace NeoLawaSAT;

    uint32_t ts = millis();

    size_t a = random(22);
    // a = 1;
    switch (a) {
    case 0:
        RunSort(strip, "Selection Sort", SelectionSort, -42);
        break;
    case 1:
        RunSort(strip, "Insertion Sort", InsertionSort, -21);
        break;
    case 2:
        RunSort(strip, "Bubble Sort", BubbleSort, -62);
        break;
    case 3:
        RunSort(strip, "Cocktail-Shaker Sort", CocktailShakerSort, -52);
        break;
    case 4:
        // RunSort(strip, "QuickSort (LR)\nHoare", QuickSortLR, r * 500);
        RunSort(strip, "QuickSort (LR)\nHoare", QuickSortLR, -21);
        break;
    case 5:
        RunSort(strip, "QuickSort (LL)\nLomoto", QuickSortLL, 500);
        break;
    case 6:
        RunSort(strip, "QuickSort\nDual Pivot", QuickSortDualPivot, 2000);
        break;
    case 7:
        RunSort(strip, "MergeSort", MergeSort, 8000);
        break;
    case 8:
        RunSort(strip, "ShellSort", ShellSort, 800);
        break;
    case 9:
        RunSort(strip, "HeapSort", HeapSort, 100);
        break;
    case 10:
        RunSort(strip, "CycleSort", CycleSort, -30);
        break;
    case 11:
        RunSort(strip, "RadixSortMSD", RadixSortMSD, 1000);
        break;
    case 12:
        RunSort(strip, "RadixSortLSD", RadixSortLSD, 18000);
        break;
    case 13:
        RunSort(strip, "std::sort", StdSort, 1000);
        break;
    case 14:
        RunSort(strip, "std::stable_sort", StdStableSort, 2000);
        break;
    case 15:
        RunSort(strip, "WikiSort", WikiSort, 50);
        break;
    case 16:
        RunSort(strip, "TimSort", TimSort, 2000);
        break;
    case 17:
        RunSort(strip, "BozoSort", BozoSort);
        break;

#define TRunHash(A, T) \
    RunHash(strip, #A, A, T)

    case 18:
        TRunHash(LinearProbingHT, 1200);
        break;
    case 19:
        TRunHash(QuadraticProbingHT, 5000);
        break;
    case 20:
        TRunHash(CuckooHashingTwo, 5000);
        break;
    case 21:
        TRunHash(CuckooHashingThree, 5000);
        break;

        // case 22:
        //     RunLawaSAT(strip);
        //     break;
    }
#undef TRunSort
}

} // namespace NeoAnimation

#endif // !NEOANIMATION_ANIMATION_RANDOMALGORITHM_HEADER

/******************************************************************************/
