// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#pragma once

#include <vector>

#include "SocketWrapper/SocketWrapper.h"
#include "Types/Pool.h"
#include "Types/PoolMessage.h"

class PoolCommunication
{
  public:
    PoolCommunication(const std::vector<Pool> pools);

    ~PoolCommunication();

    /* Open socket and connect to one of the pools */
    void login();

    /* Close current socket connection */
    void logout();

    /* Get the next job */
    Job getJob();

    /* Submit a *valid* share to the pool. */
    void submitShare(
        const std::vector<uint8_t> &hash,
        const std::string jobID,
        const uint32_t nonce);

    /* Triggers us to start listening for messages and handling them */
    void startManaging();

    /* Register a function to call when a new job is discovered */
    void onNewJob(const std::function<void(const Job &job)>);

    /* Register a function to call when a share is accepted */
    void onHashAccepted(const std::function<void(const std::string &shareID)>);

    /* Register a function to call when the current pool is disconnected and 
       a new pool is connected */
    void onPoolSwapped(const std::function<void(const Pool &pool)>);

    /* Register a function to call when the current pool is disconnected */
    void onPoolDisconnected(const std::function<void(void)>);

    /* Prints the currently connected pool for formatting purposes */
    void printPool();

    /* Gets the algorithm to use for the current pool */
    std::shared_ptr<IHashingAlgorithm> getMiningAlgorithm();

    /* Gets the name of the current algorithm */
    std::string getAlgorithmName();

    /* Whether we should use nicehash style nonces */
    const bool isNiceHash();

  private:
    /* Connect to pools when necessary */
    void managePools();

    void registerHandlers();

    /* The current pool we are connected to */
    Pool m_currentPool;

    /* All the pools available to connect to */
    const std::vector<Pool> m_allPools;

    /* The socket instance for the pool we are talking to */
    std::shared_ptr<sockwrapper::SocketWrapper> m_socket;

    /* Are we currently mining on the preferred pool? If we aren't, we will
       try reconnecting to more preferred pools periodically */
    bool m_preferredPool;

    /* The current job to be working on */
    Job m_currentJob;

    /* We call this callback every time a new job is given to us */
    std::function<void(const Job &job)> m_onNewJob;

    /* We call this callback every time the pool accepts one of our shares */
    std::function<void(const std::string &shareID)> m_onHashAccepted;

    /* We call this callback every time we change pools */
    std::function<void(const Pool &pool)> m_onPoolSwapped;

    /* We call this callback every time we disconnect from the pool */
    std::function<void(void)> m_onPoolDisconnected;

    /* Used to trigger a pool re-login attempt */
    std::condition_variable m_findNewPool;

    /* Used along with m_findNewPool */
    bool m_shouldFindNewPool = false;

    /* Manages connecting to other pools */
    std::thread m_managerThread;

    /* Handle stopping the manager thread */
    std::atomic<bool> m_shouldStop;

    /* Handle signaling between threads */
    std::mutex m_mutex;
};
