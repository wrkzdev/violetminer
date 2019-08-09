// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

/////////////////////////////////////
#include "ArgonVariants/Argon2Hash.h"
/////////////////////////////////////

void Argon2Hash::init()
{
    return;
}

std::vector<uint8_t> Argon2Hash::hash(std::vector<uint8_t> input)
{
    std::vector<uint8_t> salt(input.begin(), input.begin() + m_saltLength);
    return m_argonInstance.Hash(input, salt);
}

Argon2Hash::Argon2Hash(
    const uint32_t memoryKB,
    const uint32_t iterations,
    const uint32_t threads,
    const uint32_t saltLength,
    const Constants::ArgonVariant variant):
    m_argonInstance(variant, {}, {}, iterations, memoryKB, threads, 32),
    m_saltLength(saltLength)
{
}
