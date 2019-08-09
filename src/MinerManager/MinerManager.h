// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#pragma once

#include <memory>

#include <thread>

#include "PoolCommunication/PoolCommunication.h"
#include "Types/IHashingAlgorithm.h"

class MinerManager
{
  public:
    /* CONSTRUCTOR */
    MinerManager(
        const std::shared_ptr<PoolCommunication> pool,
        const std::function<std::shared_ptr<IHashingAlgorithm>(void)> algorithmGenerator,
        const uint32_t threadCount);

    /* DESTRUCTOR */
    ~MinerManager();

    /* PUBLIC METHODS */
    void start();

    void stop();

  private:

    /* PRIVATE METHODS */
    void hash();

    /* PRIVATE VARIABLES */
    std::vector<std::thread> m_threads;

    /* Should we stop the worker funcs */
    std::atomic<bool> m_shouldStop = false;

    /* Number of threads to launch */
    const uint32_t m_threadCount;

    /* This function supplies us an instance of the hashing algorithm instance
       each time it is called, so we can have one per thread */
    const std::function<std::shared_ptr<IHashingAlgorithm>(void)> m_algorithmGenerator;

    /* Pool connection */
    const std::shared_ptr<PoolCommunication> m_pool;

    std::atomic<uint64_t> m_hashes = 0;

    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;
};
