#pragma once

#include <algorithm>
#include <cmath>
#include <ostream>
#include <utility>

namespace wheel {

// Copy assignment, so it is recommended that T be a small object
template <typename T>
struct Vector2D {
    T x, y;

    Vector2D(T x = 0, T y = 0) : x(x), y(y) {}
    Vector2D(const Vector2D& other) : x(other.x), y(other.y) {}
    Vector2D(Vector2D&& other) : x(other.x), y(other.y) {}

    Vector2D(std::pair<T, T> p) : x(p.first), y(p.second) {}
    // template <typename U> requires std::is_convertible_v<U, T>
    // operator Vector2D<U>() const { return {static_cast<U>(x), static_cast<U>(y)}; }

    Vector2D& operator=(Vector2D other) { x = other.x; y = other.y; return *this; }

    Vector2D operator+(this Vector2D self, Vector2D other) { return { self.x + other.x, self.y + other.y }; }
    Vector2D operator-(this Vector2D self, Vector2D other) { return { self.x - other.x, self.y - other.y }; }
    Vector2D operator*(this Vector2D self, Vector2D other) { return { self.x * other.x, self.y * other.y }; }
    Vector2D operator/(this Vector2D self, Vector2D other) { return { self.x / other.x, self.y / other.y }; }
    Vector2D& operator+=(Vector2D other) { x += other.x; y += other.y; return *this; }
    Vector2D& operator-=(Vector2D other) { x -= other.x; y -= other.y; return *this; }
    Vector2D& operator*=(Vector2D other) { x *= other.x; y *= other.y; return *this; }
    Vector2D& operator/=(Vector2D other) { x /= other.x; y /= other.y; return *this; }

    bool operator<(this Vector2D self, Vector2D other) { return (self.x < other.x) && (self.y < other.y); }
    bool operator>(this Vector2D self, Vector2D other) { return (self.x > other.x) && (self.y > other.y); }
    bool operator<=(this Vector2D self, Vector2D other) { return (self.x <= other.x) && (self.y <= other.y); }
    bool operator>=(this Vector2D self, Vector2D other) { return (self.x >= other.x) && (self.y >= other.y); }
    bool operator==(this Vector2D self, Vector2D other) { return (self.x == other.x) && (self.y == other.y); }
    bool operator!=(this Vector2D self, Vector2D other) { return (self.x != other.x) || (self.y != other.y); }

    bool is_zero(this Vector2D self) { return self == 0; }

    Vector2D clamp(this Vector2D self, T x_min, T x_max, T y_min, T y_max) {
        return {
            std::clamp(self.x, x_min, x_max),
            std::clamp(self.y, y_min, y_max)
        };
    }
    void clamp_(this Vector2D self, T x_min, T x_max, T y_min, T y_max) {
        self.x = std::clamp(self.x, x_min, x_max);
        self.y = std::clamp(self.y, y_min, y_max);
    }

    Vector2D normalize(this Vector2D self) {
        if (self == 0) return self;
        T length = std::sqrt(self.x * self.x + self.y * self.y);
        return {self.x / length, self.y / length};
    }
    void normalize_(this Vector2D self) {
        if (self == 0) return;
        T length = std::sqrt(self.x * self.x + self.y * self.y);
        self.x /= length;
        self.y /= length;
    }

    T distance(this Vector2D self) {
        return std::sqrt(self.x * self.x + self.y * self.y);
    }
    T distance(this Vector2D self, Vector2D other) {
        return std::sqrt((self.x - other.x) * (self.x - other.x) + (self.y - other.y) * (self.y - other.y));
    }

    T abs(this Vector2D self) {
        return std::abs(self.x) + std::abs(self.y);
    }

    std::string to_string() const { return "[" + std::to_string(x) + "," + std::to_string(y) + "]"; }
    static Vector2D from_string(const std::string& str) {
        size_t pos = str.find(',');
        return {
            std::stof(str.substr(1, pos - 1)),
            std::stof(str.substr(pos + 1, str.size() - pos - 2))
        };
    }
    friend std::ostream& operator<<(std::ostream& os, Vector2D v) { return os << v.to_string(); }
    friend std::istream& operator>>(std::istream& is, Vector2D v) {
        std::string str;
        is >> str;
        v.from_string(str);
        return is;
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

    Rect<T> merge(const Rect<T>& other) const {
        T new_x0 = std::min(this->x0, other.x0);
        T new_y0 = std::min(this->y0, other.y0);
        T new_x1 = std::max(this->x1, other.x1);
        T new_y1 = std::max(this->y1, other.y1);
        return {new_x0, new_y0, new_x1, new_y1};
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
