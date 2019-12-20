#include <iostream>
#include <cassert>
#include <fstream>

#include "FontFreeType.h"
#include "FontAtlas.h"
#include "ccUTF8.h"
#include "Label.h"

#include "config.h"



static  std::string dir_name(const char* path);


std::shared_ptr<GlyphBitmap> test_get_glyphbitmap(FontFreeType* font_path, const char* ch);

void test_font_atlas(const std::string& chars, const char* fontFile, std::string& outdir);

void test_label(const char* font, const char* text);

int main(int argc, char** argv)
{
    const char* font_path = nullptr;
    if (argc > 1)
        font_path = argv[1];
    else
        font_path = DEFAULT_FONTPATH;


    std::string output = dir_name(argv[0]);
    
    //test_font_atlas("abcdefghijklmnopqrst", font_path, output);

    test_label(font_path, "hello\nsdfafsdf\nABAVAVAVAV\n3456767454");
    
    return 0;
}


void test_label(const char* font, const char* text)
{
    Label* label = new Label();
    //label->init(font, text, 180, 0);
    label->init(font, text, 40, 0);
    delete label;
}

std::shared_ptr<GlyphBitmap> test_get_glyphbitmap(FontFreeType &font, const char* ch)
{
    std::u32string output;
    StringUtils::UTF8ToUTF32(ch, output);
    auto glyph = font.getGlyphBitmap(output[0]);
    assert(glyph);
    return glyph;
}


void test_font_atlas(const std::string& chars, const char *fontFile,  std::string& dir)
{
    FontFreeType font(fontFile, 20.0, 0.0);
    font.loadFont();
    std::cout << "FontName: " << font.getFontFamily() << std::endl;

    FontAtlas* atlas = new FontAtlas(PixelMode::A8, 512, 512);
    atlas->init();

    for(int c =0; c < chars.size(); c++) 
    {
        auto bitmap = test_get_glyphbitmap(font, chars.c_str() + c);
        atlas->addLetter(chars[c], bitmap);
    }

    std::fstream dataFile;
    std::string filename = dir + "/output2.txt";
    dataFile.open(filename.c_str(), std::fstream::out);
#ifdef ENABLE_INSPECT
    atlas->frameAt(0).inspect(dataFile);
#endif
    dataFile.close();
    printf("write to file: %s\n", filename.c_str());

    delete atlas;


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