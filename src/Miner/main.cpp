// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#include <iostream>

#include "PoolCommunication/PoolCommunication.h"
#include "Types/Pool.h"

int main()
{
    std::vector<Pool> pools;

    pools.push_back({ "127.0.0.1", 3333 });

    PoolCommunication pool(pools);

    pool.login();
}
