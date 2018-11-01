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

#define TRunSort(A, T) \
    RunSort(strip, #A, A, T)
#define DRunSort(A) \
    RunSort(strip, #A, A)

        size_t a = random(22);
        switch (a) {
        case 0:
            TRunSort(SelectionSort, -10);
            break;
        case 1:
            TRunSort(InsertionSort, -3);
            break;
        case 2:
            TRunSort(BubbleSort, -8);
            break;
        case 3:
            TRunSort(CocktailShakerSort, -8);
            break;
        case 4:
            TRunSort(QuickSortLR, 500);
            break;
        case 5:
            TRunSort(QuickSortDualPivot, 2000);
            break;
        case 6:
            TRunSort(MergeSort, 8000);
            break;
        case 7:
            TRunSort(ShellSort, 800);
            break;
        case 8:
            TRunSort(HeapSort, 100);
            break;
        case 9:
            TRunSort(CycleSort, -30);
            break;
        case 10:
            TRunSort(RadixSortMSD, 1000);
            break;
        case 11:
            TRunSort(RadixSortLSD, 18000);
            break;
        case 12:
            TRunSort(StdSort, 1000);
            break;
        case 13:
            TRunSort(StdStableSort, 2000);
            break;
        case 14:
            TRunSort(WikiSort, 50);
            break;
        case 15:
            TRunSort(TimSort, 2000);
            break;
        case 16:
            DRunSort(BozoSort);
            break;

#define TRunHash(A, T) \
    RunHash(strip, #A, A, T)

        case 17:
            TRunHash(LinearProbingHT, 1200);
            break;
        case 18:
            TRunHash(QuadraticProbingHT, 5000);
            break;
        case 19:
            TRunHash(CuckooHashingTwo, 5000);
            break;
        case 20:
            TRunHash(CuckooHashingThree, 5000);
            break;

            // case 21:
            //     RunLawaSAT(strip);
            //     break;
        }
#undef DRunSort
    }
}

} // namespace NeoAnimation

#endif // !NEOANIMATION_ANIMATION_RANDOMALGORITHM_HEADER

/******************************************************************************/
