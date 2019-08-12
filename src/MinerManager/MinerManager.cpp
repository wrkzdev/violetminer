// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

//////////////////////////////////////
#include "MinerManager/MinerManager.h"
//////////////////////////////////////

#include <iostream>
#include <sstream>
#include <iomanip>

#include "Utilities/ColouredMsg.h"
#include "Utilities/Utilities.h"

MinerManager::MinerManager(
    const std::shared_ptr<PoolCommunication> pool,
    const uint32_t threadCount):
    m_pool(pool),
    m_threadCount(threadCount),
    m_hashManager(pool),
    m_gen(m_device())
{
}

MinerManager::~MinerManager()
{
    stop();
}

void MinerManager::setNewJob(const Job &job)
{
    /* Set new nonce */
    m_nonce = m_distribution(m_gen);

    /* Update stored job */
    m_currentJob = job;

    /* Indicate to each thread that there's a new job */
    for (int i = 0; i < m_newJobAvailable.size(); i++)
    {
        m_newJobAvailable[i] = true;
    }

    m_pool->printPool();

    /* Let the user know we got a new job */
    std::cout << WhiteMsg("New job, diff ") << WhiteMsg(job.shareDifficulty) << std::endl;
}

void MinerManager::start()
{
    m_shouldStop = false;

    /* Login to the pool */
    m_pool->login();

    /* Get the initial job to work on */
    m_currentJob = m_pool->getJob();

    m_pool->printPool();
    std::cout << WhiteMsg("New job, diff ") << WhiteMsg(m_currentJob.shareDifficulty) << std::endl;

    /* Set initial nonce */
    m_nonce = m_distribution(m_gen);

    /* Indicate that there's no new jobs available to other threads */
    m_newJobAvailable = std::vector<bool>(m_threadCount, false);

    /* Launch off the miner threads */
    for (uint32_t i = 0; i < m_threadCount; i++)
    {
        m_threads.push_back(std::thread(&MinerManager::hash, this, i));
    }

    /* Launch off the thread to print stats regularly */
    m_statsThread = std::thread(&MinerManager::printStats, this);

    /* Hook up the function to set a new job when it arrives */
    m_pool->onNewJob([this](const Job &job){
        setNewJob(job);
    });

    /* Pass through accepted shares to the hash manager */
    m_pool->onHashAccepted([this](const auto &){
        m_hashManager.shareAccepted();
    });

    m_pool->onPoolSwapped([this](const Pool &newPool){
        resumeMining();
    });

    m_pool->onPoolDisconnected([this](){
        pauseMining();
    });

    /* Start listening for messages from the pool */
    m_pool->startManaging();
}

void MinerManager::resumeMining()
{
    m_shouldStop = false;

    std::cout << WhiteMsg("Resuming mining.") << std::endl;

    m_currentJob = m_pool->getJob();

    m_pool->printPool();
    std::cout << WhiteMsg("New job, diff ") << WhiteMsg(m_currentJob.shareDifficulty) << std::endl;

    /* Set initial nonce */
    m_nonce = m_distribution(m_gen);

    /* Indicate that there's no new jobs available to other threads */
    m_newJobAvailable = std::vector<bool>(m_threadCount, false);
 
    /* Launch off the miner threads */
    for (uint32_t i = 0; i < m_threadCount; i++)
    {
        m_threads.push_back(std::thread(&MinerManager::hash, this, i));
    }

    /* Launch off the thread to print stats regularly */
    m_statsThread = std::thread(&MinerManager::printStats, this);
}

void MinerManager::pauseMining()
{
    std::cout << WhiteMsg("Pausing mining.") << std::endl;

    m_shouldStop = true;

    for (int i = 0; i < m_threadCount; i++)
    {
        m_newJobAvailable[i] = true;
    }

    /* Wait for all the threads to stop */
    for (auto &thread : m_threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    if (m_statsThread.joinable())
    {
        m_statsThread.join();
    }

    /* Empty the threads vector for later re-creation */
    m_threads.clear();
}

void MinerManager::stop()
{
    m_shouldStop = true;

    for (int i = 0; i < m_threadCount; i++)
    {
        m_newJobAvailable[i] = true;
    }

    /* Wait for all the threads to stop */
    for (auto &thread : m_threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }

    /* Empty the threads vector for later re-creation */
    m_threads.clear();

    /* Wait for the stats thread to stop */
    if (m_statsThread.joinable())
    {
        m_statsThread.join();
    }

    /* Close the socket connection to the pool */
    m_pool->logout();
}

void MinerManager::hash(uint32_t threadNumber)
{
    std::shared_ptr<IHashingAlgorithm> algorithm = m_pool->getMiningAlgorithm();

    const bool isNiceHash = m_pool->isNiceHash();

    /* Let the algorithm perform any necessary initialization */
    algorithm->init(m_currentJob.rawBlob);

    while (!m_shouldStop)
    {
        /* Offset the nonce by our thread number so each thread has an individual
           nonce */
        uint32_t localNonce = m_nonce + threadNumber;

        Job job = m_currentJob;

        /* If nicehash mode is enabled, we are only allowed to alter 3 bytes
           in the nonce, instead of four. The first byte is reserved for nicehash
           to do with as they like.
           To achieve this, we wipe the top byte (localNonce & 0x00FFFFFF) of
           local nonce. We then wipe the bottom 3 bytes of job.nonce
           (*job.nonce() & 0xFF000000). Finally, we and them together, so the
           top byte of the nonce is reserved for nicehash.
           See further https://github.com/nicehash/Specifications/blob/master/NiceHash_CryptoNight_modification_v1.0.txt
           Note that the above specification indicates that the final byte of
           the nonce is reserved, but in fact it is the first byte that is 
           reserved. */
        if (isNiceHash)
        {
            *job.nonce() = (localNonce & 0x00FFFFFF) | (*job.nonce() & 0xFF000000);
        }
        else
        {
            *job.nonce() = localNonce;
        }

        /* Allow the algorithm to reinitialize for the new round, as some algorithms
           can avoid reinitializing each round. For example, we can calculate
           the salt once per job, and cache it. */
        algorithm->reinit(job.rawBlob);

        while (!m_newJobAvailable[threadNumber])
        {
            const auto hash = algorithm->hash(job.rawBlob);

            m_hashManager.submitHash(hash, job.jobID, *job.nonce(), job.target);

            localNonce += m_threadCount;

            if (isNiceHash)
            {
                *job.nonce() = (localNonce & 0x00FFFFFF) | (*job.nonce() & 0xFF000000);
            }
            else
            {
                *job.nonce() = localNonce;
            }
        }

        /* Switch to new job. */
        m_newJobAvailable[threadNumber] = false;
    }
}

void MinerManager::printStats()
{
    while (!m_shouldStop)
    {
        Utilities::sleepUnlessStopping(std::chrono::seconds(20), m_shouldStop);
        m_hashManager.printStats();
    }
}
