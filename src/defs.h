#pragma once

#include <memory>
#include <vector>

template<typename D>
class Vec2 {
public:
    Vec2(D x, D y);
    Vec2() = default;
    Vec2(const Vec2<D>&);
    D getX() const { return _x; }
    D getY() const { return _y; }
    D setX(float x) { _x = x; }
    D setY(float y) { _y = y; }

    D asWidth() const { return _x; }
    D asHeight() const { return _y; }
    D setWidth(float x) { _x = x; }
    D setHeight(float y) { _y = y; }

    void set(D x, D y) { _x = x; _y = y; }
private:
    D _x = 0;
    D _y = 0;
    friend class Rect;
};

template<typename D>
Vec2<D>::Vec2(D x, D y) : _x(x), _y(y)
{
}

template<typename D>
Vec2<D>::Vec2(const Vec2<D>& o) : _x(o._x), _y(o._y)
{
}


class Rect {
public:
    typedef int ScalarType;
    typedef Vec2<ScalarType> Vec2Impl;
    Rect(ScalarType x, ScalarType y, ScalarType w, ScalarType h);
    Rect() = default;
    Rect(const Vec2Impl& origin, const Vec2Impl& size);
    Rect(const Rect&);
    ScalarType getLeft() const { return _origin._x; };
    ScalarType getRight() const { return _origin._x + _size._x; }
    ScalarType getTop() const { return _origin._y + _size._y; }
    ScalarType getBottom() const { return _origin._y; }

    ScalarType getHeight() const { return _size.asHeight(); }
    ScalarType getWidth() const { return _size.asWidth(); }

    const Vec2Impl& getOrigin() const { return _origin; }
    const Vec2Impl& getSize() const { return _size; }

    void setOrigin(const Vec2Impl& origin);
    void setSize(const Vec2Impl& size);

    void setOrigin(ScalarType x, ScalarType y);
    void setSize(ScalarType w, ScalarType h);

private:
    Vec2Impl _origin;
    Vec2Impl _size;
};


enum class PixelMode {
    AI88,
    A8,
    RGB888,
    BGRA8888,
    INVAL,
};

int PixelModeSize(PixelMode mode);

class GlyphBitmap {
public:
    GlyphBitmap(std::vector<uint8_t>&& data, int width, int height, Rect rect, int xAdvance, PixelMode mode);

    GlyphBitmap(GlyphBitmap&& other) noexcept;

    void inspect() const;

private:
    int _width = 0;
    int _height = 0;
    std::vector<uint8_t> _data;
    Rect _rect;
    int _xAdvance = 0;
    PixelMode _pixelMode;
};
