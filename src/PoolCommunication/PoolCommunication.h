// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#pragma once

#include <vector>

#include "Types/Pool.h"

class PoolCommunication
{
  public:
    PoolCommunication(const std::vector<Pool> pools);

    void login();

  private:
    Pool m_currentPool;

    const std::vector<Pool> m_allPools;
};
