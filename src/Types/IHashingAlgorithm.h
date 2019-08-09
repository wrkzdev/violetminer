// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#pragma once

#include <vector>

class IHashingAlgorithm
{
  public:
    virtual void init() = 0;

    virtual std::vector<uint8_t> hash(std::vector<uint8_t> input) = 0;

    virtual ~IHashingAlgorithm() {};
};
