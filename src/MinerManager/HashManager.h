// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#pragma once

#include <chrono>
#include <memory>
#include <vector>

#include "PoolCommunication/PoolCommunication.h"

class HashManager
{
  public:
    HashManager(const std::shared_ptr<PoolCommunication> pool);

    /* Call this to submit a hash to the pool. We will check the diff. */
    void submitHash(
        const std::vector<uint8_t> &hash,
        const std::string jobID,
        const uint32_t nonce,
        const uint64_t target);

    /* Call this when a share got accepted by the pool. */
    void shareAccepted();

    /* Print the current stats */
    void printStats();
    
  private:
    /* Total number of hashes we have performed */
    std::atomic<uint64_t> m_totalHashes = 0;

    /* Total number of hashes we have submitted (that are above the difficulty) */
    std::atomic<uint64_t> m_submittedHashes = 0;

    /* Total number of submitted hashes that were accepted by the pool */
    std::atomic<uint64_t> m_acceptedHashes = 0;

    const std::shared_ptr<PoolCommunication> m_pool;

    /* TODO: Hashrate will break when the dev pool pauses this timer. */
    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
};
