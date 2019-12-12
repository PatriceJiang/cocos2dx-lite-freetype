#include <iostream>
#include <cassert>

#include "FontFreeType.h"
#include "ccUTF8.h"

#include "config.h"

void test_get_glyphbitmap(const char* data, int ch);

int main(int argc, char** argv)
{
    const char* font_path = nullptr;
    if (argc > 1)
        font_path = argv[1];
    else
        font_path = DEFAULT_FONTPATH;

    test_get_glyphbitmap(font_path, 'k');
    return 0;
}


void test_get_glyphbitmap(const char* data, int ch)
{
    FontFreeType font(data, 80.0, 0.0);

    assert(font.loadFont());

    std::cout << "FontName: " << font.getFontFamily() << std::endl;
    std::u32string output;
    StringUtils::UTF8ToUTF32("r", output);
    auto glyph = font.getGlyphBitmap(output[0]);
    assert(glyph);
    glyph->inspect();
}