#include "FontFreetype.h"
#include "Utils.h"

#include <cassert>

class FontFreeTypeLibrary {
public:
    FontFreeTypeLibrary() {
        memset(&_library, 0, sizeof(FT_Library));
        FT_Init_FreeType(&_library);
    }
    ~FontFreeTypeLibrary()
    {
        FT_Done_FreeType(_library);
    }

    FT_Library * get() { return &_library; }

private:
    FT_Library _library;
};

namespace {
    std::weak_ptr<FontFreeTypeLibrary> _sFTLibrary;

    PixelMode FTtoPixelModel(FT_Pixel_Mode mode)
    {
        switch (mode)
        {
        case FT_PIXEL_MODE_GRAY:
            return PixelMode::A8;
        case FT_PIXEL_MODE_LCD:
            return PixelMode::RGB888;
        case FT_PIXEL_MODE_BGRA:
            return PixelMode::BGRA8888;
        default:
            assert(false); //invalidate pixelmode
            return PixelMode::INVAL;
        }
    }

}


FontFreeType::FontFreeType(const std::string& fontName, float fontSize, float outline)
{
    _ftLibrary = _sFTLibrary.lock();
    if (!_ftLibrary)
    {
        _ftLibrary = std::make_shared< FontFreeTypeLibrary>();
    }

    _fontName = fontName;
    _fontSize = fontSize;
    _outlineSize = outline;
    
    if (_outlineSize > 0)
    {
        FT_Stroker_New(getFTLibrary(), &_stroker);
        FT_Stroker_Set(_stroker, (int)(_outlineSize * 64),
            FT_STROKER_LINECAP_ROUND,
            FT_STROKER_LINEJOIN_ROUND,
            0);
    }
}

FontFreeType::~FontFreeType()
{
    if (_stroker) FT_Stroker_Done(_stroker);
    if (_face) FT_Done_Face(_face);
}

FT_Library& FontFreeType::getFTLibrary()
{
    return *_ftLibrary->get();
}

bool FontFreeType::loadFont()
{
    _fontData = utils::readFile(_fontName);

    if (FT_New_Memory_Face(getFTLibrary(), _fontData.data(), _fontData.size(), 0, &_face))
    {
        return false;
    }

    if (FT_Select_Charmap(_face, _encoding))
    {
        int foundIndex = -1;
        for (int charmapIndex = 0; charmapIndex < _face->num_charmaps; charmapIndex++)
        {
            if (_face->charmaps[charmapIndex]->encoding != FT_ENCODING_NONE)
            {
                foundIndex = charmapIndex;
                break;
            }
        }
        if (foundIndex == -1)
        {
            return false;
        }
        _encoding = _face->charmaps[foundIndex]->encoding;
        if (FT_Select_Charmap(_face, _encoding))
        {
            return false;
        }
    }

    //default dpi 
    const int DPI = 72;
    int fontSizeInPoints = (int)(64.0f * _fontSize); //TODO times CC_CONTENT_SCALE_FACTOR;

    if (FT_Set_Char_Size(_face, fontSizeInPoints, fontSizeInPoints, DPI, DPI))
    {
        return false;
    }

    _lineHeight = (_face->size->metrics.ascender - _face->size->metrics.descender) >> 6;

    return true;
}

int FontFreeType::getHorizontalKerningForChars(uint64_t a, uint64_t b) const
{
    auto idx1 = FT_Get_Char_Index(_face, static_cast<FT_ULong>(a));
    if (!idx1)
        return 0;
    auto idx2 = FT_Get_Char_Index(_face, static_cast<FT_ULong>(b));
    if (!idx2)
        return 0;
    FT_Vector kerning;
    if (FT_Get_Kerning(_face, idx1, idx2, FT_KERNING_DEFAULT, &kerning))
        return 0;
    
    return static_cast<int>(kerning.x >> 6);
}

std::unique_ptr<std::vector<int>> FontFreeType::getHorizontalKerningForUTF32Text(const std::u32string& text) const
{
    if (!_face) return nullptr;
    if (FT_HAS_KERNING(_face) == 0) return nullptr;

    const auto letterNum = text.length();
    std::vector<int>* sizes = new std::vector<int>(letterNum, 0);

    for (int i = 1; i < letterNum; i++)
    {
        (*sizes)[i] = getHorizontalKerningForChars(text[i - 1], text[i]);
    }
    return std::unique_ptr<std::vector<int>>(sizes);
}


int FontFreeType::getFontAscender() const
{
    return static_cast<int>(_face->size->metrics.ascender >> 6);
}

const char* FontFreeType::getFontFamily() const
{
    if (!_face) return nullptr;
    return _face->family_name;
}

std::unique_ptr<GlyphBitmap> FontFreeType::getGlyphBitmap(uint64_t ch)
{
    if (!_face) return nullptr;
    const auto load_char_flag = FT_LOAD_RENDER | FT_LOAD_NO_AUTOHINT;
    if (FT_Load_Char(_face, static_cast<FT_ULong>(ch), load_char_flag))
    {
        return nullptr;
    }

    auto& metrics = _face->glyph->metrics;
    int x = metrics.horiBearingX >> 6;
    int y = -(metrics.horiBearingY >> 6);
    int w = metrics.width >> 6;
    int h = metrics.height >> 6;

    int adv = metrics.horiAdvance >> 6;

    auto& bitmap = _face->glyph->bitmap;
    int bmWidth = bitmap.width;
    int bmHeight = bitmap.rows;
    PixelMode mode = FTtoPixelModel(static_cast<FT_Pixel_Mode>(bitmap.pixel_mode));
    int size = PixelModeSize(mode) * bmWidth * bmHeight;
    std::vector<uint8_t> data((uint8_t*)bitmap.buffer, (uint8_t*)bitmap.buffer + size);
    auto* ret = new GlyphBitmap(std::move(data), bmWidth, bmHeight, Rect(x, y, w, h), adv, mode);
    return std::unique_ptr<GlyphBitmap>(ret);
}
