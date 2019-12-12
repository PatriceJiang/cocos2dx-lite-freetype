#include "Utils.h"

#include <cstdio>
#include <cstdlib>

namespace utils
{

    std::vector<uint8_t> readFile(const std::string &path)
    {
        FILE *fp = fopen(path.c_str(), "rb");
        fseek(fp, 0, SEEK_END);
        int size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        std::vector<uint8_t> data;
        data.reserve(size);
        uint8_t *buffer = new uint8_t[4096]; // 4K
        int got = 0;

        while ((got = fread(buffer, 1, 4096, fp)) > 0) {
            data.insert(data.end(), buffer, buffer + got);
        }

        delete[]buffer;
        return std::move(data);
    }

}