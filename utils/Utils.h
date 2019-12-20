#pragma once


#include <vector>
#include <string>
#include <stdint.h>
#include <iosfwd>

namespace utils
{
    std::vector<uint8_t> readFile(const std::string &path);

    void inspectData(std::ostream& out, int width, int height, int pixelBytes, const std::vector<uint8_t>& data);
};
