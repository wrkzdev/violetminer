// Copyright (c) 2019, The TurtleCoin Developers
//
// Please see the included LICENSE file for more information.

#include <string>
#include <vector>

namespace Utilities
{
    void removeCharFromString(std::string &str, const char c);

    void trim(std::string &str);

    void leftTrim(std::string &str);

    void rightTrim(std::string &str);

    bool startsWith(const std::string &str, const std::string &substring);

    std::vector<std::string> split(const std::string &str, char delimiter);

    std::string removePrefix(const std::string &str, const std::string &prefix);

    int char2int(char input);

    void toHex(const unsigned char *input, char *output, size_t inputLength);

    std::string toHex(const uint32_t &input);

    std::string toHex(const std::vector<uint8_t> &input);

    std::vector<uint8_t> fromHex(const std::string &input);

    void fromHex(const char *input, size_t inputLength, unsigned char *output);

} // namespace Utilities
