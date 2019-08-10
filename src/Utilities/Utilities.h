// Copyright (c) 2019, Zpalmtree
//
// Please see the included LICENSE file for more information.

#pragma once

#include <atomic>
#include <chrono>

namespace Utilities
{
    void sleepUnlessStopping(const std::chrono::milliseconds duration, std::atomic<bool> &condition);
}
