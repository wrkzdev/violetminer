// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#pragma once

#include <vector>

#include "SocketWrapper/SocketWrapper.h"
#include "Types/Pool.h"

class PoolCommunication
{
  public:
    PoolCommunication(const std::vector<Pool> pools);

    void login();

  private:
    /* The current pool we are connected to */
    Pool m_currentPool;

    /* All the pools available to connect to */
    const std::vector<Pool> m_allPools;

    /* The socket instance for the pool we are talking to */
    std::shared_ptr<sockwrapper::SocketWrapper> m_socket;
};
