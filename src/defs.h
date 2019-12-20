#pragma once

#include <memory>
#include <vector>

#include <iosfwd>

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

    Vec2<D> operator +(const Vec2<D>& o) const;
    Vec2<D> operator -(const Vec2<D>& o) const;

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


template<typename D>
Vec2<D> Vec2<D>::operator +(const Vec2<D>& o) const
{
    return Vec2<D>(_x + o._x, _y + o._y);
}

template<typename D>
Vec2<D> Vec2<D>::operator -(const Vec2<D>& o) const
{
    return Vec2<D>(_x - o._x, _y - o._y);
}

template<typename D>
class Vec3 {
public:
    Vec3(D x, D y, D z);
    Vec3() = default;
    Vec3(const Vec3<D>&);
    D getX() const { return _x; }
    D getY() const { return _y; }
    D getZ() const { return _z; }
    D setX(float x) { _x = x; }
    D setY(float y) { _y = y; }
    D setZ(float z) { _z = z; }

    void set(D x, D y, D z) { _x = x; _y = y; _z = z; }
private:
    D _x = 0;
    D _y = 0;
    D _z = 0;
};

template<typename D>
Vec3<D>::Vec3(D x, D y, D z) : _x(x), _y(y), _z(z)
{
}

template<typename D>
Vec3<D>::Vec3(const Vec3<D>& o) : _x(o._x), _y(o._y), _z(o._z)
{
}

template<typename D>
class Vec4 {
public:
    Vec4(D x, D y, D z, D k);
    Vec4() = default;
    Vec4(const Vec4<D>&);
    D getX() const { return _x; }
    D getY() const { return _y; }
    D getZ() const { return _z; }
    D getK() const { return _k; }
    D setX(float x) { _x = x; }
    D setY(float y) { _y = y; }
    D setZ(float z) { _z = z; }
    D setK(float k) { _k = k; }

    void set(D x, D y, D z, D k) { _x = x; _y = y; _z = z; _k = k; }
private:
    D _x = 0;
    D _y = 0;
    D _z = 0;
    D _k = 0;
};

template<typename D>
Vec4<D>::Vec4(D x, D y, D z, D k) : _x(x), _y(y), _z(z), _k(k)
{
}

template<typename D>
Vec4<D>::Vec4(const Vec4<D>& o) : _x(o._x), _y(o._y), _z(o._z), _k(o._k)
{
}


class Rect {
public:
    typedef float ScalarType;
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
#ifdef ENABLE_INSPECT
    void inspect(std::ostream &out) const;
#endif

    int getWidth() const { return _width; }
    int getHeight() const { return _height; }
    Rect getRect() const { return _rect; }
    int getXAdvance() const { return _xAdvance; }
    PixelMode getPixelMode() const { return _pixelMode; }
    std::vector<uint8_t>& getData() { return _data; }

private:
    int _width = 0;
    int _height = 0;
    std::vector<uint8_t> _data;
    Rect _rect;
    int _xAdvance = 0;
    PixelMode _pixelMode;
};
