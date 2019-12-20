#pragma once

#include "FontFreetype.h"

#include <unordered_map>
#include <algorithm>

struct FontLetterDefinition
{
    float texX = 0, texY =0;
    float texWidth = 0, texHeight = 0;
    //int offsetX, offsetY;
    Rect rect;
    int textureID = -1;
    int xAdvance = 0;
    bool validate = false;
};

class FontAtlasFrame
{
public:

    enum class FrameResult {
        SUCCESS,
        E_FULL,
        E_ERROR
    };

    FontAtlasFrame() = default;
    FontAtlasFrame(FontAtlasFrame&); //move 
    void init(PixelMode mode, int width, int height);
    FrameResult append(int width, int height, std::vector<uint8_t> &, Rect &out);


    int getWidth() const { return _WIDTH; }
    int getHeight() const { return _HEIGHT; }

#ifdef ENABLE_INSPECT
    void inspect(std::ostream& out) const;
#endif

private:

    inline int remainRowXSpace() const { return _WIDTH - _currentRowX; }
    inline int remainYSpace() const { return _HEIGHT - _currentRowY; }
    inline bool hasRowXSpace(int x) const { return x <= remainRowXSpace(); }
    inline bool hasYSpace(int y) const { return y <= remainYSpace(); }
    inline bool hasNextRowXSpace(int x) const { return x <= _WIDTH; }
    inline bool hasNextRowYSpace(int y) const { return y <= remainYSpace() - _currRowHeight; }
    
    bool prepareRow(int width, int height);

    void moveToNextRow() 
    {
        _currentRowY += _currRowHeight;
        _currentRowX = 0;
        _currRowHeight = 0;
    }

    inline void moveToNextCursor(int width, int height)
    {
        _currRowHeight = std::max(_currRowHeight, height);
        _currentRowX += width;
    }

    std::vector<uint8_t> _buffer;
    //internal states
    int _WIDTH          = 0;
    int _HEIGHT         = 0;
    int _currentRowY    = 0;
    int _currentRowX    = 0;
    int _currRowHeight  = 0;
    PixelMode _pixelMode = PixelMode::A8;
 
};

class FontAtlas {

public:

    FontAtlas(PixelMode mode, int width, int height);
    virtual ~FontAtlas();

    bool init();

    bool addLetter(uint64_t ch, std::shared_ptr<GlyphBitmap> bitmap);

    FontLetterDefinition* getOrLoad(uint64_t ch, FontFreeType* font);
    
    FontAtlasFrame& frameAt(int idx);
private:

    void addLetterDef(uint64_t ch, std::shared_ptr<GlyphBitmap> bitmap, const Rect& rect);

    std::unordered_map<uint64_t, FontLetterDefinition> _letterMap;

    FontAtlasFrame   _textureFrame;
    std::vector<FontAtlasFrame> _buffers;
    int _textureBufferIndex =   0;
    int _width              =   0;
    int _height             =   0;
    PixelMode _pixelMode    =   PixelMode::A8;
};