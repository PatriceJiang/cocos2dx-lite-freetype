#include "Utils.h"

#include <cstdio>
#include <cstdlib>

#include <iostream>

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


    void inspectData(std::ostream& out, int width, int height, int pixelBytes, const std::vector<uint8_t>& data)
    {
        out << "{";

        const int LEFT = 0;
        const int RIGHT = width;
        const int TOP = height;
        const int BOTTOM = 0;

        for (int i = BOTTOM; i < TOP; i++)
        {
            out << "{";
            for (int j = LEFT; j < RIGHT; j++)
            {
                if(pixelBytes == 1)
                {
                    out << "{" << (int)(data[j + i * width]) << "}";
                }
                else
                {
                    out << "{";
                    for (int k = 0; k << pixelBytes; k++) {
                        out << (int)data[(j + i * width) * pixelBytes + k];
                        if (k != pixelBytes - 1) {
                            out << ",";
                        }
                    }
                    out << "}";
                }


                if (j != RIGHT - 1)
                {
                    out << ",";
                }
            }
            out << "}";
            if (i != TOP - 1) {
                out << ",";
            }
            out.flush();
        }

        out << "}";
    }
}