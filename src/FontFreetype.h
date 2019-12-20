#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_STROKER_H


#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "defs.h"

class FontFreeTypeLibrary;

class FontFreeType
{
public:
    FontFreeType(const std::string& fontName, float fontSize, float outline);
    virtual ~FontFreeType();

    FT_Library& getFTLibrary();

    bool loadFont();

    int getHorizontalKerningForChars(uint64_t a, uint64_t b) const;
    std::unique_ptr<std::vector<int>> getHorizontalKerningForUTF32Text(const std::u32string &text) const;

    int getFontAscender() const;
    const char* getFontFamily() const;

    std::shared_ptr<GlyphBitmap> getGlyphBitmap(uint64_t ch);

private:
    std::shared_ptr<FontFreeTypeLibrary> _ftLibrary;
    std::vector<uint8_t> _fontData;
    float _outlineSize = 0.0f;
    float _fontSize = 0.0f;
    float _lineHeight = 0.0f;
    std::string _fontName;

    FT_Stroker _stroker = { 0 };
    FT_Face    _face = { 0 };
    FT_Encoding _encoding = FT_ENCODING_UNICODE;
};