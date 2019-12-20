#pragma once

#include <string>
#include <FontAtlas.h>
#include <FontFreetype.h>


enum class LabelAlignmentH
{
    LEFT, CENTER, RIGHT
};
enum class LabelAlignmentV {
    TOP, CENTER, BOTTOM
};

struct C3F_T2F_C4B {
    Vec3<float> vertex;
    Vec2<float> texCoord;
    Vec4<uint8_t> color;
};

class Label {
public:
    Label() = default;
    bool init(const std::string& font, const std::string& text, float fontSize, float outline);
    virtual ~Label();

protected:
    bool updateContent();
    
private:
    std::string _string;
    std::u32string _u32string;
    std::string _font;
    float         _fontSize   = 0;
    float         _outline  = 0;
    FontAtlas* _fontAtlas   = nullptr;
    FontFreeType* _ttfFont  = nullptr;
    int         _spaceX     = 0;
    int     _lineHeight     = 0;
    LabelAlignmentV _alignV = LabelAlignmentV::CENTER;
    LabelAlignmentH _alignH = LabelAlignmentH::CENTER;
    bool        _enableKerning = true;
};