// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#pragma once

#include <memory>

#include <thread>

#include "PoolCommunication/PoolCommunication.h"
#include "Types/HashingAlgorithm.h"

class MinerManager
{
  public:
    /* CONSTRUCTOR */
    MinerManager(
        const std::shared_ptr<PoolCommunication> pool,
        const HashingAlgorithm hashingAlgorithm,
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

    /* The core hash function */
    const HashingAlgorithm m_hashingAlgorithm;

    /* Pool connection */
    const std::shared_ptr<PoolCommunication> m_pool;
};
