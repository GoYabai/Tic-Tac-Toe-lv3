#include <iostream>
#include <cassert>
#include <random>
#include <vector>
#include "src/core/logic.h"
#include "src/core/types.h"

void runPropertyBasedTest()
{
    std::cout << "[PBT] Starting Property-based Test for countSymbol...\n";
    std::mt19937 rng(42);
    int test_cases = 1000;
    int passed = 0;

    for (int i = 0; i <= test_cases; ++i)
    {
        int size = 3 + (rng() % 10);
        Board b = core::initBoard(size);

        char symbols[] = {EMPTY_CELL, 'X', 'O'};
        std::uniform_int_distribution<int> sym_dist(0, 2);
        for (int r = 0; r < size; r++)
        {
            for (int c = 0; c < size; c++)
            {
                b.grid[r][c] = symbols[sym_dist(rng)];
            }
        }

        auto validMoves = core::enumerateValidMoves(b);
        if (validMoves.empty())
        {
            continue;
        }

        std::uniform_int_distribution<int> move_dist(0, validMoves.size() - 1);
        Move random_move = validMoves[move_dist(rng)];
        char random_sym = (rng() % 2 == 0) ? 'X' : 'O';

        int count_before = core::countSymbol(b, random_sym);
        Board next_b = core::applyMove(b, random_move, random_sym);
        int count_after = core::countSymbol(next_b, random_sym);

        assert(count_after == count_before + 1);

        passed++;
    }

    std::cout << "[PBT] SUCCESS! Passed " << passed << " random board states.\n";
    std::cout << "[PBT] Mathematical invariant strictly holds true!\n";
}

int main()
{
    runPropertyBasedTest();
    return 0;
}