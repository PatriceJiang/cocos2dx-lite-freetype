#include <iostream>
#include <cassert>
#include <fstream>

#include "FontFreeType.h"
#include "ccUTF8.h"

#include "config.h"


static  std::string dir_name(const char* path);


void test_get_glyphbitmap(std::string & outdir, const char* data, int ch);

int main(int argc, char** argv)
{
    const char* font_path = nullptr;
    if (argc > 1)
        font_path = argv[1];
    else
        font_path = DEFAULT_FONTPATH;


    std::string output = dir_name(argv[0]);

    test_get_glyphbitmap(output, font_path, 'k');
    return 0;
}



void test_get_glyphbitmap(std::string &dir, const char* data, int ch)
{
    std::fstream dataFile;
    FontFreeType font(data, 80.0, 0.0);

    assert(font.loadFont());

    std::cout << "FontName: " << font.getFontFamily() << std::endl;
    std::u32string output;
    StringUtils::UTF8ToUTF32("r", output);
    auto glyph = font.getGlyphBitmap(output[0]);
    assert(glyph);

    std::string filename = dir + "/output.txt";
    dataFile.open(filename.c_str(), std::fstream::out);
#ifdef ENABLE_INSPECT
    glyph->inspect(dataFile);
#endif
    dataFile.close();
    printf("write to file: %s\n", filename.c_str());
}



std::string dir_name(const char* path)
{
    const char* e = path + strlen(path) - 1;
    while (*e != '/' && *e != '\\' && e != path) {
        e--;
    }

    if (*e == '/' || *e == '\\')
    {
        return std::string(path, e - path);
    }
    return "";
}