#pragma once

#include <algorithm>
#include <cmath>
#include <ostream>

namespace wheel {

template <typename T>
struct Vector2D {
    T x, y;

    Vector2D operator+(const Vector2D& other) const { return { x + other.x, y + other.y }; }
    Vector2D operator-(const Vector2D& other) const { return { x - other.x, y - other.y }; }
    Vector2D operator*(T scalar) const { return { x * scalar, y * scalar}; }
    Vector2D operator/(T scalar) const { return { x / scalar, y / scalar}; }
    Vector2D& operator+=(const Vector2D& other) { x += other.x; y += other.y; return *this; }
    Vector2D& operator-=(const Vector2D& other) { x -= other.x; y -= other.y; return *this; }
    Vector2D& operator*=(T scalar) { x *= scalar; y *= scalar; return *this; }
    Vector2D& operator/=(T scalar) { x /= scalar; y /= scalar; return *this; }
    bool operator==(const Vector2D& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Vector2D& other) const { return x != other.x || y != other.y; }
    bool is_zero() const { return x == 0 && y == 0; }
    friend std::ostream& operator<<(std::ostream& os, const Vector2D& v) { return os << "(" << v.x << ", " << v.y << ")"; }

    template <typename U>
    operator Vector2D<U>() const {
        return {static_cast<U>(x), static_cast<U>(y)};
    }

    Vector2D& clamp(T x_min, T x_max, T y_min, T y_max) {
        x = std::clamp(x, x_min, x_max);
        y = std::clamp(y, y_min, y_max);
        return *this;
    }

    Vector2D normalize() {
        if (x == 0 && y == 0) {
            return *this;
        }

        T length = std::sqrt(x * x + y * y);
        return {x / length, y / length};
    }

    T distance(const Vector2D& other) const {
        return std::sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
    }
};

template <typename T>
struct Rect {
    Rect() {}
    Rect(T x0, T y0, T x1, T y1) : x0(x0), y0(y0), x1(x1), y1(y1) {}
    Rect(const Vector2D<T>& position, const Vector2D<T>& size) {
        x0 = position.x - size.x / 2;
        y0 = position.y - size.y / 2;
        x1 = position.x + size.x / 2;
        y1 = position.y + size.y / 2;
    }

    friend std::ostream& operator<<(std::ostream& os, const Rect& rect) {
        return os << "(" << rect.x0 << ", " << rect.y0 << ", " << rect.x1 << ", " << rect.y1 << ")";
    }

    bool operator==(const Rect<T>& rect) {
        return x0 == rect.x0 && y0 == rect.y0 && x1 == rect.x1 && y1 == rect.y1;
    }

    T intersection(const Rect<T>& other) {
        T x0 = std::max(this->x0, other.x0);
        T y0 = std::max(this->y0, other.y0);
        T x1 = std::min(this->x1, other.x1);
        T y1 = std::min(this->y1, other.y1);
        if (x0 < x1 && y0 < y1) {
            return (x1 - x0) * (y1 - y0);
        }
        return 0;
    }

    bool contains(const Vector2D<T>& point) const {
        return x0 <= point.x && point.x <= x1 && y0 <= point.y && point.y <= y1;
    }

    bool contains(const Rect<T>& rect) const {
        return x0 <= rect.x0 && y0 <= rect.y0 && x1 >= rect.x1 && y1 >= rect.y1;
    }
    
    bool is_overlapping(const Rect<T>& other) const {
        return x0 < other.x1 && x1 > other.x0 && y0 < other.y1 && y1 > other.y0;
    }

    bool is_zero() const {
        return *this == Rect<T>{};
    }

    template <typename U>
    operator Rect<U>() const {
        return {static_cast<U>(x0), static_cast<U>(y0), static_cast<U>(x1), static_cast<U>(y1)};
    }

    Vector2D<T> center() const { return {(x0 + x1) / 2, (y0 + y1) / 2}; }
    Vector2D<T> left_top() const { return {x0, y0}; }
    Vector2D<T> size() const { return {x1 - x0, y1 - y0}; } 

    T x0 = 0;
    T y0 = 0;
    T x1 = 0;
    T y1 = 0;
};

}  // namespace wheel
