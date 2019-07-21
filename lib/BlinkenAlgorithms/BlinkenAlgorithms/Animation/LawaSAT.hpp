/*******************************************************************************
 * lib/BlinkenAlgorithms/BlinkenAlgorithms/Animation/LawaSAT.hpp
 *
 * Lawa - Lazy Walksat Local search
 * Author: Tomas Balyo (tomas.balyo@kit.edu)
 * Karlsruhe Institure of Technology
 *
 * Copyright (C) 2017-2018 Tomas Balyo <tomas.balyo@kit.edu>
 *
 * All rights reserved. Published under the GNU General Public License v3.0
 ******************************************************************************/

#ifndef BLINKENALGORITHMS_ANIMATION_LAWASAT_HEADER
#define BLINKENALGORITHMS_ANIMATION_LAWASAT_HEADER

#include <BlinkenAlgorithms/Animation/Sort.hpp>
#include <BlinkenAlgorithms/Color.hpp>

#include <cmath>
#include <cstdlib>
#include <ctype.h>
#include <stdio.h>
#include <vector>

namespace BlinkenLawaSAT {

using namespace BlinkenSort;

class Lawa
{
private:
    int numVariables;
    int numClauses;
    Item* tValues = array.data() - 1;
    Item* satLits = array.data() + 80;

    struct Clause {
        int numLits;
        int8_t lits[3];
    };

    Clause* clauses;

    std::vector<int>* posOccList;
    std::vector<int>* negOccList;
    std::vector<int> unsatClauseIds;

public:
    void printClause(const Clause& cls) {
        for (int li = 0; li < cls.numLits; li++) {
            // Serial.printf("%d ", cls.lits[li]);
        }
        // Serial.printf("0 \n");
    }

    void printModel() {
        for (int var = 1; var <= numVariables; var++) {
            // Serial.printf("%d ", tValues[var].value_);
        }
        // Serial.printf("\n");
    }

    void printDebug() {
        for (int ci = 0; ci < numClauses; ci++) {
            // Serial.printf("%d: ", ci);
            printClause(clauses[ci]);
        }
        printModel();
    }

    bool is_true(int lit) { return tValues[abs(lit)].value_ == lit; }

    void initializeSearch() {
        // tValues = new int[numVariables + 1];
        // satLits = new char[numClauses];

        for (int var = 1; var <= numVariables; var++) {
            tValues[var] = Item(rand() % 2 == 0 ? var : -var);
        }

        for (int ci = 0; ci < numClauses; ci++) {
            Clause& cls = clauses[ci];
            char slits = 0;
            for (int li = 0; li < cls.numLits; li++) {
                int lit = clauses[ci].lits[li];
                if (is_true(lit))
                    slits++;
            }
            if (slits == 0) {
                unsatClauseIds.push_back(ci);
            }
            satLits[ci] = Item(slits);
        }
    }

    int computeMakeScore(int lit) {
        int score = 0;
        std::vector<int>& occList =
            lit > 0 ? posOccList[lit] : negOccList[-lit];
        for (int cid : occList) {
            if (satLits[cid].value_ == 0) {
                score++;
            }
        }
        return score;
    }

    int computeBreakScore(int lit) {
        int score = 0;
        std::vector<int>& occList =
            lit > 0 ? negOccList[lit] : posOccList[-lit];
        for (int cid : occList) {
            if (satLits[cid].value_ == 1) {
                score++;
            }
        }
        return score;
    }

    void flipLiteral(int lit) {
        int var = abs(lit);
        std::vector<int>& decOccList =
            lit > 0 ? negOccList[var] : posOccList[var];
        std::vector<int>& incOccList =
            lit > 0 ? posOccList[var] : negOccList[var];

        for (int cid : decOccList) {
            satLits[cid]--;
            if (satLits[cid].value_ == 0) {
                unsatClauseIds.push_back(cid);
            }
        }
        for (int cid : incOccList) {
            satLits[cid]++;
        }

        tValues[var] = Item(lit);
    }

    void search() {
        unsigned long round = 0;
        while (unsatClauseIds.size() > 0) {
            round++;
            if (round >= 50000)
                break;
            // Serial.printf("c round %lu, unsat clauses: %lu\n", round,
            // unsatClauseIds.size());
            int usidid = rand() % unsatClauseIds.size();
            int usid = unsatClauseIds[usidid];
            unsatClauseIds[usidid] = unsatClauseIds.back();
            unsatClauseIds.pop_back();
            Clause& cls = clauses[usid];
            if (satLits[usid].value_ > 0) {
                continue;
            }
            // Serial.printf("c will satisfy clause ");
            // printClause(cls);

            int id1 = rand() % cls.numLits;
            int id2 = rand() % cls.numLits;
            while (id1 == id2) {
                id1 = rand() % cls.numLits;
                id2 = rand() % cls.numLits;
            }
            int lit1 = cls.lits[id1];
            int lit2 = cls.lits[id2];

            int score1 = computeMakeScore(lit1) -
                         computeBreakScore(lit1); // - flippedCount[abs(lit1)];
            int score2 = computeMakeScore(lit2) -
                         computeBreakScore(lit2); // - flippedCount[abs(lit2)];

            if (score1 == score2) {
                // Serial.printf("c flipping %d and %d, their score: %d\n",
                // lit1, lit2, score1);
                flipLiteral(lit1);
                flipLiteral(lit2);
            }
            else {
                int toFlip = score1 > score2 ? lit1 : lit2;
                // Serial.printf("c flipping %d, its score is max (%d, %d)\n",
                // toFlip, score1, score2);
                flipLiteral(toFlip);
            }
        }

        size_t unsatCount = 0;
        for (int i = 0; i < numClauses; ++i) {
            if (satLits[i].value_ == 0)
                ++unsatCount;
        }

        // Serial.printf("c searched finished after %lu rounds unsats %u.\n",
        //               round, unsatCount);
    }

    void makeOccurrenceLists() {
        posOccList = new std::vector<int>[1 + numVariables];
        negOccList = new std::vector<int>[1 + numVariables];
        for (int ci = 0; ci < numClauses; ci++) {
            for (int li = 0; li < clauses[ci].numLits; li++) {
                int lit = clauses[ci].lits[li];
                int var = abs(lit);
                if (lit > 0) {
                    posOccList[var].push_back(ci);
                }
                else {
                    negOccList[var].push_back(ci);
                }
            }
        }
    }

    void randomFla(int vars) {
        numVariables = vars;
        numClauses = int(4.2 * vars);

        clauses = new Clause[numClauses];
        for (int i = 0; i < numClauses; i++) {
            clauses[i].numLits = 3;
            // clauses[i].lits = new int[3];
            int v1 = (1 + (rand() % vars));
            int v2 = (1 + (rand() % vars));
            int v3 = (1 + (rand() % vars));
            while (v1 == v2 || v1 == v3 || v2 == v3) {
                v2 = (1 + (rand() % vars));
                v3 = (1 + (rand() % vars));
            }
            clauses[i].lits[0] = v1 * (rand() % 2 == 0 ? 1 : -1);
            clauses[i].lits[1] = v2 * (rand() % 2 == 0 ? 1 : -1);
            clauses[i].lits[2] = v3 * (rand() % 2 == 0 ? 1 : -1);
        }
    }

    void Run() {
        randomFla(80);
        makeOccurrenceLists();
        initializeSearch();
        search();
        // Serial.printf("s SATISFIABLE\nv ");
        printModel();

        delete[] clauses;
        delete[] posOccList;
        delete[] negOccList;
        std::vector<int>().swap(unsatClauseIds);
    }
};

/******************************************************************************/

template <typename LEDStrip>
class LawaAnimation : public SortAnimation<LEDStrip>
{
public:
    using Super = SortAnimation<LEDStrip>;
    using Super::strip_;

    LawaAnimation(LEDStrip& strip) : Super(strip) { }

    unsigned intensity_high = 255;
    unsigned intensity_low = 64;

    void OnAccess(const Item* a, bool with_delay) override {
        if (a < array.data() || a >= array.data() + array_size)
            return;
        flash(a - array.data(), with_delay);
    }

    void flash_low(size_t i) {
        if (i < 80) {
            // variables: >= black are negative values.
            if (array[i].value_ >= unsigned_negative)
                strip_.setPixel(i, Color(0));
            else
                strip_.setPixel(i, Color(intensity_low));
        }
        else {
            // clauses
            if (array[i].value_ == 0) {
                strip_.setPixel(i, Color(255, 0, 0));
            }
            // else if (array[i].value_ == 1) {
            //     strip_.setPixel(i, Color(96, 96, 0));
            // }
            // else if (array[i].value_ == 2) {
            //     strip_.setPixel(i, Color(0, 128, 0));
            // }
            else {
                strip_.setPixel(i, Color(0, 32, 0));
            }
        }
    }

    void flash_high(size_t i) {
        flash_low(i);
    }

    void flash(size_t i, bool with_delay = true) {
        if (!with_delay)
            return flash_low(i);

        flash_high(i);

        if (!strip_.busy())
            strip_.show();

        delay_micros(100);
        if (DelayHook)
            DelayHook();

        flash_low(i);
    }

    void pflush() {
        strip_.show();
    }
};

template <typename LEDStrip>
void RunLawaSAT(LEDStrip& strip) {
    LawaAnimation<LEDStrip> ani(strip);
    if (AlgorithmNameHook)
        AlgorithmNameHook("SAT Solver\nLazy Walk");
    ani.array_black();
    Lawa().Run();
}

} // namespace BlinkenLawaSAT

#endif // !BLINKENALGORITHMS_ANIMATION_LAWASAT_HEADER

/******************************************************************************/
