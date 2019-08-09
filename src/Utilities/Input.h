// Copyright (c) 2019, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#include <string>

namespace Utilities
{
    bool confirm(const std::string &msg);

    bool confirm(const std::string &msg, const bool defaultReturn);

    bool parseAddressFromString(std::string &host, uint16_t &port, std::string address);
} // namespace Utilities
