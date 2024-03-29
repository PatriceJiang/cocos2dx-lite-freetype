#include "defs.h"

#include "Utils.h"

#include <cassert>
#include <cstdarg>
#include <iostream>

Rect::Rect(ScalarType x, ScalarType y, ScalarType w, ScalarType h)
    : _origin(x, y), _size(w, h)
{}

Rect::Rect(const Vec2Impl& origin, const Vec2Impl& size) :
    _origin(origin), _size(size)
{}

Rect::Rect(const Rect& ref) :
    _origin(ref._origin), _size(ref._size)
{}

void Rect::setOrigin(const Vec2Impl& origin)
{
    _origin = origin;
}

void Rect::setSize(const Vec2Impl& size)
{
    _size = size;
}

void Rect::setOrigin(ScalarType x, ScalarType y)
{
    _origin.set(x, y);
}

void Rect::setSize(ScalarType w, ScalarType h)
{
    _size.set(w, h);
}

GlyphBitmap::GlyphBitmap(std::vector<uint8_t>&& data, int width, int height, Rect rect, int adv, PixelMode mode)
    : _data(data), _width(width), _height(height), _rect(rect), _xAdvance(adv), _pixelMode(mode)
{
}
GlyphBitmap::GlyphBitmap(GlyphBitmap&& other) noexcept
{
    _data = std::move(other._data);
    _rect = other._rect;
    _width = other._width;
    _height = other._height;
    _xAdvance = other._xAdvance;
    _pixelMode = other._pixelMode;
}

#ifdef ENABLE_INSPECT
void GlyphBitmap::inspect(std::ostream& out) const
{
    printf("width: %d\n", _width);
    printf("height: %d\n", _height);
    printf("pixelSize: %d\n", PixelModeSize(_pixelMode));
    printf("rect{x: %d, y:%d, w:%d, h:%d}\n",
        _rect.getOrigin().getX(),
        _rect.getOrigin().getY(),
        _rect.getSize().asWidth(),
        _rect.getSize().asHeight()
    );

    utils::inspectData(out, _width, _height, PixelModeSize(_pixelMode), _data);
}
#endif

int PixelModeSize(PixelMode mode)
{
    switch (mode)
    {
    case PixelMode::AI88:
        return 2;
    case PixelMode::A8:
        return 1;
    case PixelMode::RGB888:
        return 3;
    case PixelMode::BGRA8888:
        return 4;
    default:
        assert(false); // invalidate pixel mode
    }
    return 0;
}

