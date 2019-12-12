#pragma once


#include <vector>
#include <string>
#include <stdint.h>

namespace utils
{
    std::vector<uint8_t> readFile(const std::string &path);
};
