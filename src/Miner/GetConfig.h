// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#pragma once

#include <string>
#include <thread>

#include "Types/Pool.h"

struct MinerConfig
{
    std::vector<Pool> pools;

    uint32_t threadCount = std::thread::hardware_concurrency();

    std::string configLocation;
};

void to_json(nlohmann::json &j, const MinerConfig &config);

void from_json(const nlohmann::json &j, MinerConfig &config);

Pool getPool();

std::vector<Pool> getPools();

MinerConfig getConfigInteractively();

MinerConfig getConfigFromJSON(const std::string &configLocation);

MinerConfig getMinerConfig(int argc, char **argv);
