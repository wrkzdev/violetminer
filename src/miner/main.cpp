// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#include <iostream>

#include <chrono>

#include <functional>

#include <vector>

#include "Argon2/Argon2.h"
#include "Argon2/Constants.h"

#include "Blake2/Blake2b.h"

int main()
{
    const std::vector<uint8_t> chukwaInput = {
        1, 0, 251, 142, 138, 200, 5, 137, 147, 35, 55, 27, 183, 144, 219, 25,
        33, 138, 253, 141, 184, 227, 117, 93, 139, 144, 243, 155, 61, 85, 6,
        169, 171, 206, 79, 169, 18, 36, 69, 0, 0, 0, 0, 238, 129, 70, 212, 159,
        169, 62, 231, 36, 222, 181, 125, 18, 203, 198, 198, 243, 185, 36, 217,
        70, 18, 124, 122, 151, 65, 143, 147, 72, 130, 143, 15, 2
    };

    const std::vector<uint8_t> chukwaSalt(chukwaInput.begin(), chukwaInput.begin() + 16);

    const auto chukwaExpected = "c0dad0eeb9c52e92a1c3aa5b76a3cb90bd7376c28dce191ceeb1096e3a390d2e";

    Argon2 chukwa(Constants::ARGON2ID, {}, {}, 3, 512, 1, 32);

    const int iterations = 5000;

    std::cout << "Running benchmark, " << iterations << " iterations." << std::endl;

    const auto begin = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; i++)
    {
        /* So the hashes are always different */
        std::vector<uint8_t> input = chukwaInput;
        input.push_back(i);

        chukwa.Hash(input, chukwaSalt);
    }

    const auto elapsedTime = std::chrono::high_resolution_clock::now() - begin;

    std::cout << "Benchmark results: " << (iterations / std::chrono::duration_cast<std::chrono::seconds>(elapsedTime).count()) << " H/s\n";
}
