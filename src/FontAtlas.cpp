#include "FontAtlas.h"
#include <cassert>
#include "Utils.h"

FontAtlasFrame::FontAtlasFrame(FontAtlasFrame& o)
{
    // move buffer instead of copy
    std::swap(_buffer, o._buffer);
    _WIDTH = o._WIDTH;
    _HEIGHT = o._HEIGHT;
    _currentRowX = o._currentRowX;
    _currentRowY = o._currentRowY;
    _currRowHeight = o._currRowHeight;
    _pixelMode = o._pixelMode;
}

void FontAtlasFrame::init(PixelMode pixelMode, int width, int height)
{
    _pixelMode = pixelMode;
    _WIDTH = width;
    _HEIGHT = height;
    _currentRowX = 0;
    _currentRowY = 0;
    _currRowHeight = 0;
    _buffer.resize(PixelModeSize(pixelMode) * width * height);
    std::fill(_buffer.begin(), _buffer.end(), 0);
}

FontAtlasFrame::FrameResult FontAtlasFrame::append(int width, int height, std::vector<uint8_t> &data, Rect &out)
{
    assert(_buffer.size() > 0);
    assert(width <= _WIDTH && height <= _HEIGHT);
    bool hasSpace = prepareRow(width, height);
    if (!hasSpace) {
        return FrameResult::E_FULL;
    }
    
    //update sub-data
    const int pixelSize = PixelModeSize(_pixelMode);
    uint8_t* dst = _buffer.data();
    uint8_t* src = data.data();
    uint8_t* dstOrigin = pixelSize * (_currentRowY * _WIDTH + _currentRowX)  + dst;
    const int BytesEachRow = pixelSize * width;
    for (int i = 0; i < height; i++)
    {
        memcpy(dstOrigin + i * _WIDTH * pixelSize, src + i * BytesEachRow, BytesEachRow);
    }

    out.setOrigin(_currentRowX, _currentRowY);
    out.setSize(width, height);

    // move cursor
    moveToNextCursor(width, height);

    return FrameResult::SUCCESS;

}

bool FontAtlasFrame::prepareRow(int width, int height)
{
    if (hasRowXSpace(width) && hasYSpace(height) ) {
        return true;
    }
    if (hasNextRowXSpace(width) && hasNextRowYSpace(height))
    {
        moveToNextRow();
        return true;
    }
    return false;
}



#ifdef ENABLE_INSPECT
void FontAtlasFrame::inspect(std::ostream& out) const
{
    printf("FontAtlasFrame: (%d,%d)", _WIDTH, _HEIGHT);
    utils::inspectData(out, _WIDTH, _HEIGHT, PixelModeSize(_pixelMode), _buffer);
}
#endif


FontAtlas::FontAtlas(PixelMode pixelMode, int width, int height)
    :_pixelMode(pixelMode), _width(width), _height(height)
{
}

FontAtlas::~FontAtlas() {}


bool FontAtlas::init() 
{
    int pixelSize = PixelModeSize(_pixelMode);
    _textureFrame.init(_pixelMode, _width, _height);
    _letterMap.clear();
    return true;
}

bool FontAtlas::addLetter(uint64_t ch, std::shared_ptr<GlyphBitmap> bitmap)
{
    Rect rect;
    FontAtlasFrame::FrameResult ret = _textureFrame.append(bitmap->getWidth(), bitmap->getHeight(), bitmap->getData(), rect);

    switch (ret) {
    case FontAtlasFrame::FrameResult::E_ERROR:
        //TODO: ERROR LOG
        assert(false);
        return false;
    case FontAtlasFrame::FrameResult::E_FULL:
        // Allocate a new frame & add bitmap the frame
        _buffers.emplace_back(_textureFrame);
        _textureBufferIndex += 1;
        _textureFrame.init(_pixelMode, _width, _height);
        return addLetter(ch, bitmap);
    case FontAtlasFrame::FrameResult::SUCCESS:
        addLetterDef(ch, bitmap, rect);
        return true;
    default:
        //TODO: LOG
        assert(false);
    }
    return false;
}

void FontAtlas::addLetterDef(uint64_t ch, std::shared_ptr<GlyphBitmap> bitmap, const Rect& rect)
{
    assert(bitmap->getPixelMode() == _pixelMode);

    auto& def = _letterMap[ch];
    def.validate = true;
    def.textureID = _textureBufferIndex;
    def.xAdvance = bitmap->getXAdvance();
    def.rect = bitmap->getRect();
    def.texX = 1.0f * rect.getOrigin().getX() / _textureFrame.getWidth();
    def.texY = 1.0f * rect.getOrigin().getY() / _textureFrame.getHeight();
    def.texWidth = 1.0f * rect.getWidth() / _textureFrame.getWidth();
    def.texHeight = 1.0f * rect.getHeight() / _textureFrame.getHeight();
}


FontLetterDefinition* FontAtlas::getOrLoad(uint64_t ch, FontFreeType* font)
{
    auto it = _letterMap.find(ch);
    if (it != _letterMap.end()) return &it->second;

    if (font) {
        auto bitmap = font->getGlyphBitmap(ch);
        if (bitmap) {
            if (addLetter(ch, bitmap)) {
                return getOrLoad(ch, nullptr);
            }
        }
    }
    return nullptr;
}


FontAtlasFrame& FontAtlas::frameAt(int idx)
{
    return idx == _textureBufferIndex ? _textureFrame : _buffers.at(idx);
}