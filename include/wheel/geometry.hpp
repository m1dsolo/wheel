#pragma once

#include <algorithm>
#include <cmath>
#include <array>
#include <ostream>
#include <string>
#include <tuple>

namespace wheel {

template <size_t N, typename T>
struct Vector {
    std::array<T, N> data;

    Vector() : data{} {}
    Vector(T value) { std::fill(data.begin(), data.end(), value); }

    template <typename... Args>
    requires (sizeof...(Args) == N)
    Vector(Args&&... args) : data{std::forward<Args>(args)...} {}

    T& operator[](size_t index) { return data[index]; }
    const T& operator[](size_t index) const { return data[index]; }

    Vector<N, T> operator+(const Vector<N, T>& other) const {
        Vector<N, T> result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = data[i] + other.data[i];
        }
        return result;
    }

    Vector<N, T> operator-(const Vector<N, T>& other) const {
        Vector<N, T> result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = data[i] - other.data[i];
        }
        return result;
    }

    Vector<N, T> operator*(const Vector<N, T>& other) const {
        Vector<N, T> result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = data[i] * other.data[i];
        }
        return result;
    }

    Vector<N, T> operator/(const Vector<N, T>& other) const {
        Vector<N, T> result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = data[i] / other.data[i];
        }
        return result;
    }

    Vector<N, T>& operator+=(const Vector<N, T>& other) {
        for (size_t i = 0; i < N; ++i) {
            data[i] += other.data[i];
        }
        return *this;
    }

    Vector<N, T>& operator-=(const Vector<N, T>& other) {
        for (size_t i = 0; i < N; ++i) {
            data[i] -= other.data[i];
        }
        return *this;
    }

    Vector<N, T>& operator*=(const Vector<N, T>& other) {
        for (size_t i = 0; i < N; ++i) {
            data[i] *= other.data[i];
        }
        return *this;
    }

    Vector<N, T>& operator/=(const Vector<N, T>& other) {
        for (size_t i = 0; i < N; ++i) {
            data[i] /= other.data[i];
        }
        return *this;
    }

    bool operator==(const Vector<N, T>& other) const {
        for (size_t i = 0; i < N; ++i) {
            if (data[i] != other.data[i]) return false;
        }
        return true;
    }

    bool operator!=(const Vector<N, T>& other) const {
        return !(*this == other);
    }

    T length() const {
        T sum = 0;
        for (const auto& elem : data) {
            sum += elem * elem;
        }
        return std::sqrt(sum);
    }

    Vector<N, T> normalize() const {
        T len = length();
        if (len == 0) {
            return *this;
        }
        return *this * (1 / len);
    }

    void normalize_() {
        T len = length();
        if (len == 0) {
            return;
        }
        for (auto& val : data) {
            val /= len;
        }
    }

    T dot(const Vector<N, T>& other) const {
        T result = 0;
        for (size_t i = 0; i < N; ++i) {
            result += data[i] * other.data[i];
        }
        return result;
    }

    template <size_t M = N>
    requires (M == 2)
    T cross(const Vector<N, T>& other) const {
        return data[0] * other.data[1] - data[1] * other.data[0];
    }

    template <size_t M = N>
    requires (M == 3)
    Vector<3, T> cross(const Vector<3, T>& other) const {
        return {
            data[1] * other.data[2] - data[2] * other.data[1],
            data[2] * other.data[0] - data[0] * other.data[2],
            data[0] * other.data[1] - data[1] * other.data[0]
        };
    }

    std::string to_string() const {
        std::string str = "[";
        for (size_t i = 0; i < N; ++i) {
            str += std::to_string(data[i]);
            if (i != N - 1) str += ", ";
        }
        str += "]";
        return str;
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector<N, T>& v) {
        return os << v.to_string();
    }
};

template <size_t I, size_t N, typename T>
constexpr T get(wheel::Vector<N, T>& vec) noexcept {
    static_assert(I < N, "Index out of bounds");
    return vec.data[I];
}

template <size_t I, size_t N, typename T>
constexpr const T get(const wheel::Vector<N, T>& vec) noexcept {
    static_assert(I < N, "Index out of bounds");
    return vec.data[I];
}

template <typename T>
using Vector2D = Vector<2, T>;

template <typename T>
using Vector3D = Vector<3, T>;

template <typename T>
using Vector4D = Vector<4, T>;

template <typename T>
struct Rect {
    Rect() {}
    Rect(T x0, T y0, T x1, T y1) : x0(x0), y0(y0), x1(x1), y1(y1) {}
    Rect(const Vector2D<T>& position, const Vector2D<T>& size) {
        x0 = position[0] - size[0] / 2;
        y0 = position[1] - size[1] / 2;
        x1 = position[0] + size[0] / 2;
        y1 = position[1] + size[1] / 2;
    }
    Rect(const Rect<T>& other) : x0(other.x0), y0(other.y0), x1(other.x1), y1(other.y1) {}
    Rect(Rect&& other) : x0(other.x0), y0(other.y0), x1(other.x1), y1(other.y1) {}
    Rect& operator=(const Rect<T>& other) {
        x0 = other.x0;
        y0 = other.y0;
        x1 = other.x1;
        y1 = other.y1;
        return *this;
    }
    Rect& operator=(Rect&& other) {
        x0 = other.x0;
        y0 = other.y0;
        x1 = other.x1;
        y1 = other.y1;
        return *this;
    }

    T width() const { return x1 - x0; }
    T height() const { return y1 - y0; }

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
        return x0 <= point[0] && point[0] <= x1 && y0 <= point[1] && point[1] <= y1;
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

template <size_t N, typename T>
struct std::tuple_size<wheel::Vector<N, T>> : std::integral_constant<size_t, N> {};

template <size_t I, size_t N, typename T>
struct std::tuple_element<I, wheel::Vector<N, T>> {
    using type = T;
};
