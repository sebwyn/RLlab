#pragma once

struct Vec2 {
    int r, c;

    Vec2() = default;
    Vec2(int r, int c) : r(r), c(c) {}

    Vec2 operator+(const Vec2 &other) {
        return {r + other.r, c + other.c};
    }

    Vec2 operator-(const Vec2 &other) {
        return {r - other.r, c - other.c};
    }

    bool operator==(const Vec2 &other) {
        if (r == other.r && c == other.c)
            return true;
        else
            return false;
    }

    bool operator!=(const Vec2 &other) {
        if (r == other.r && c == other.c)
            return false;
        else
            return true;
    }

    Vec2 operator*(const int scalar) {
        return Vec2(scalar * r, scalar * c);
    }

    Vec2 operator/(const int scalar) {
        return Vec2(r / scalar, c / scalar);
    }
};

extern Vec2 north, east, west, south;
