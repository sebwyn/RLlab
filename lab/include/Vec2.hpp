#pragma once

struct Vec2 {
    int x, y;

    Vec2() = default;
    Vec2(int x, int y) : x(x), y(y) {}

    Vec2 operator+(const Vec2 &other) {
        return {x + other.x, y + other.y};
    }

    Vec2 operator-(const Vec2 &other) {
        return {x - other.x, y - other.y};
    }

    bool operator==(const Vec2 &other) {
        if (x == other.x && y == other.y)
            return true;
        else
            return false;
    }

    bool operator!=(const Vec2 &other) {
        if (x == other.x && y == other.y)
            return false;
        else
            return true;
    }

    Vec2 operator*(const int scalar) {
        return Vec2(scalar * x, scalar * y);
    }

    Vec2 operator/(const int scalar) {
        return Vec2(x / scalar, y / scalar);
    }
};

extern Vec2 north, east, west, south;
