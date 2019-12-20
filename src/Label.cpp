#include "Label.h"
#include "ccUTF8.h"

#include <cassert>

#include <fstream>

#define cout_vect(vec) \
        std::cout << "{" << vec.getX() << "," << vec.getY() << "}";

class TextSpace {
    /**
    *   Y 
    *   ^
    *   |
    *   |
    *   +------> X
    */
public:
    void fillRect(Rect &rect, Rect &uv);
    void reset();

    inline bool validate() const { return _data.size() > 0; }

    void inspect(std::ostream &) const;

    void translate(float x, float y);

    Vec2<float> center() const;

    float getWidth() const { return  _data.size() > 0 ? _right - _left : 0; }
    float getHeight() const { return _data.size() > 0 ? _top - _bottom : 0; }
private:
    float _left   = FLT_MAX;
    float _bottom = FLT_MAX;
    float _right  = FLT_MIN;
    float _top    = FLT_MIN;
    float _x = 0.0f;
    float _y = 0.0f;
    std::vector<Rect>   _data;
    std::vector<Rect>  _uv;
};

void TextSpace::reset()
{
    _left = FLT_MAX;
    _bottom = FLT_MAX;
    _right = FLT_MIN;
    _top = FLT_MIN;
    _x = 0.0f;
    _y = 0.0f;
    _data.clear();
    _uv.clear();
}

void TextSpace::fillRect(Rect &rect, Rect &uv)
{
    _left = std::min(_left, rect.getLeft());
    _right = std::max(_right, rect.getRight());
    _bottom = std::min(_bottom, rect.getBottom());
    _top = std::max(_top, rect.getTop());
    _data.push_back(rect);
    _uv.push_back(uv);
}

void TextSpace::translate(float x, float y)
{
    _x = x;
    _y = y;
}

Vec2<float> TextSpace::center() const
{
    Vec2<float> ret((_left + _right) / 2.0f, (_bottom + _top) / 2.0f);
    return ret;
}

void TextSpace::inspect(std::ostream &out) const
{

    const int size = _data.size();

    Vec2<float> offset(_x, _y);

    for (int i = 0; i < size; i++)
    {
        auto rect = _data[i];
        rect.setOrigin(rect.getOrigin() + offset);
        auto uv = _uv[i];
        out << "Triangle[{";
        out << "{" << rect.getLeft() << "," << rect.getBottom() << "}, ";
        out << "{" << rect.getRight() << "," << rect.getBottom() << "}, ";
        out << "{" << rect.getLeft() << "," << rect.getTop() << "}";
        out << "},";
        out << "VertexTextureCoordinates -> {";
        out << "{" << uv.getLeft() << ", " << uv.getBottom() << "},";
        out << "{" << uv.getRight() << ", " << uv.getBottom()<< "},";
        out << "{" << uv.getLeft() << ", " << uv.getTop() << "}";
        out << "}],";

        out << "Triangle[{";
        out << "{" << rect.getRight() << "," << rect.getBottom() << "}, ";
        out << "{" << rect.getRight() << "," << rect.getTop() << "}, ";
        out << "{" << rect.getLeft() << "," << rect.getTop() << "}";
        out << "},";
        out << "VertexTextureCoordinates -> {";
        out << "{" << uv.getRight() << ", " << uv.getBottom() << "},";
        out << "{" << uv.getRight() << ", " << uv.getTop() << "},";
        out << "{" << uv.getLeft() << ", " << uv.getTop() << "}";
        out << "}]";

        if (i != size - 1)
        {
            out << ",";
        }
        out.flush();
    }
}

bool Label::init(const std::string& font, const std::string& text, float fontSize, float outline)
{
    _ttfFont = new FontFreeType(font, fontSize, outline);
    _ttfFont->loadFont();
    _fontAtlas = new FontAtlas(PixelMode::A8, 512, 512);
    _fontAtlas->init();

    _string = text;
    _font = font;
    _fontSize = fontSize;
    _outline = outline;

    StringUtils::UTF8ToUTF32(text.c_str(), _u32string);

    _lineHeight = _ttfFont->getFontAscender();

    updateContent();
    return true;
}

Label::~Label()
{
    delete _fontAtlas;
    delete _ttfFont;
}

bool Label::updateContent()
{
    std::vector<C3F_T2F_C4B> rows;
    FontLetterDefinition* letterDef;
    Rect row;

    int contentWidth = 0;
    int contentHeight = 0;
    int rowHeight = _lineHeight;

    //TODO: kerning

    //LabelCursor cursor;
    int cursorX = 0;
    int cursorY = _lineHeight;

    //  std::cout << "Graphics[{";

    std::vector<TextSpace> spaces;

    TextSpace space;

    for (int i = 0; i < _u32string.size(); i++)
    {
        auto ch = _u32string[i];

        if (ch == u'\r')
        {
            cursorX = 0;
            continue;
        }

        if (ch == u'\n')
        {
            //  cursorY += _lineHeight;
            cursorX = 0;
            if (space.validate())
            {
                spaces.push_back(space);
                space.reset();
            }
            continue;
        }

        letterDef = _fontAtlas->getOrLoad(ch, _ttfFont);
        if (!letterDef) continue;

        //cursor.fillRect(letterDef->get)
        Rect& rect = letterDef->rect;
        int left = cursorX + rect.getLeft();
        int right = cursorX + rect.getRight();
#if 0
        int top = cursorY + rect.getBottom();
        int bottom = cursorY + rect.getTop();
#else
        int bottom = cursorY + rect.getBottom();
        int top = cursorY + rect.getTop();
#endif
        Vec2<int> lt(left, top);
        Vec2<int> lb(left, bottom);
        Vec2<int> rt(right, top);
        Vec2<int> rb(right, bottom);


        Rect letterRect(left, bottom, right - left, top - bottom);
        Rect letterTexture(letterDef->texX, letterDef->texY, letterDef->texWidth, letterDef->texHeight);

        space.fillRect(letterRect, letterTexture);



        cursorX += _spaceX + letterDef->xAdvance;
    }

#ifdef ENABLE_INSPECT
    std::fstream dataFile;
    std::string filename = "C:\\Projects\\freetype-tests\\build\\Debug\\output3.txt";
    dataFile.open(filename.c_str(), std::fstream::out);
    _fontAtlas->frameAt(0).inspect(dataFile);
    dataFile.close();
#endif

    if (space.validate())
    {
        spaces.push_back(space);
        space.reset();
    }

    std::cout << std::endl;

    float max_width = 0;
    float max_height = spaces.size() * _lineHeight;
    for (int i = 0; i < spaces.size(); i++)
    {
       auto& s = spaces[i];
       max_width = std::max(max_width, s.getWidth());
    }



    std::fstream cmdFile;
    std::string filename2 = "C:\\Projects\\freetype-tests\\build\\Debug\\output_draw.txt";
    cmdFile.open(filename2.c_str(), std::fstream::out);

    cmdFile << "Graphics[{Texture[img], ";
    for(int i=0;i<spaces.size();i++)
    {
        auto& s = spaces[i];
        auto c = s.center();
#if 1
        Vec2<float> K(0, - max_height / 2 + i * _lineHeight + _lineHeight / 2.0f);
        Vec2<float> M = K - s.center();
#else
        //left
        //Vec2<float> K(-max_width/2 + s.getWidth() / 2.0f, -max_height / 2 + i * _lineHeight + _lineHeight / 2.0f);
        //Vec2<float> M = K - s.center();

        //right
        Vec2<float> K(max_width / 2 - s.getWidth() / 2.0f, -max_height / 2 + i * _lineHeight + _lineHeight / 2.0f);
        Vec2<float> M = K - s.center();

#endif
        s.translate(M.getX(), M.getY());
        s.inspect(cmdFile);
        if (i != spaces.size() - 1)
        {
            cmdFile << ",";
        }
    }
    cmdFile << "}, Background -> LightBlue, Axes -> True, GridLines -> Automatic, ImageSize -> Large]";

    cmdFile.close();

    return true;
}
