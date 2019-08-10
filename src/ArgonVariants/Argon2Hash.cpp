// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

/////////////////////////////////////
#include "ArgonVariants/Argon2Hash.h"
/////////////////////////////////////

void Argon2Hash::init(std::vector<uint8_t> &initialInput)
{
    return;
}

/* Salt is not altered by nonce. We can initialize it once per job here. */
void Argon2Hash::reinit(const std::vector<uint8_t> &input)
{
    m_salt = std::vector<uint8_t>(input.begin(), input.begin() + m_saltLength);
}

std::vector<uint8_t> Argon2Hash::hash(std::vector<uint8_t> &input)
{
    return m_argonInstance.Hash(input, m_salt);
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
