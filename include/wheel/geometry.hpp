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
    Vector2D operator*(double scalar) const { return { x * scalar, y * scalar}; }
    Vector2D operator/(double scalar) const { return { x / scalar, y / scalar}; }
    Vector2D& operator+=(const Vector2D& other) { x += other.x; y += other.y; return *this; }
    Vector2D& operator-=(const Vector2D& other) { x -= other.x; y -= other.y; return *this; }
    Vector2D& operator*=(double scalar) { x *= scalar; y *= scalar; return *this; }
    Vector2D& operator/=(double scalar) { x /= scalar; y /= scalar; return *this; }
    bool operator==(const Vector2D& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Vector2D& other) const { return x != other.x || y != other.y; }
    bool is_zero() const { return x == 0 && y == 0; }
    friend std::ostream& operator<<(std::ostream& os, const Vector2D& v) { return os << "(" << v.x << ", " << v.y << ")"; }

    Vector2D& clamp(double x_min, double x_max, double y_min, double y_max) {
        x = std::clamp(x, x_min, x_max);
        y = std::clamp(y, y_min, y_max);
        return *this;
    }

    Vector2D normalize() {
        if (x == 0 && y == 0) {
            return *this;
        }

        double length = std::sqrt(x * x + y * y);
        return {x / length, y / length};
    }

    double distance(const Vector2D& other) const {
        return std::sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
    }
};

template <typename T>
struct Rect {
    Vector2D<T> position;
    Vector2D<T> size;

    friend std::ostream& operator<<(std::ostream& os, const Rect& rect) {
        return os << "position: " << rect.position << ", size: " << rect.size;
    }

    T intersection(const Rect<T>& other) {
        T x1 = std::max(position.x - size.x / 2, other.position.x - other.size.x / 2);
        T y1 = std::max(position.y - size.y / 2, other.position.y - other.size.y / 2);
        T x2 = std::min(position.x + size.x / 2, other.position.x + other.size.x / 2);
        T y2 = std::min(position.y + size.y / 2, other.position.y + other.size.y / 2);
        if (x2 < x1 || y2 < y1) {
            return 0;
        }
        return (x2 - x1) * (y2 - y1);
    }

    template <typename U>
    bool intersection(const Vector2D<U>& pos) {
        return pos.x >= position.x - size.x / 2 && pos.x <= position.x + size.x / 2 &&
               pos.y >= position.y - size.y / 2 && pos.y <= position.y + size.y / 2;
    }
};

}  // namespace wheel
