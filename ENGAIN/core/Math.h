#pragma once

#include <SDL2/SDL.h>

namespace ENGAIN {

struct Vector2 {
    float x, y;
    
    Vector2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
    
    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }
    
    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }
    
    Vector2 operator*(float scalar) const {
        return Vector2(x * scalar, y * scalar);
    }
    
    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
    
    Vector2& operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
};

struct Rectangle {
    float x, y, width, height;
    
    Rectangle(float x = 0.0f, float y = 0.0f, float w = 0.0f, float h = 0.0f)
        : x(x), y(y), width(w), height(h) {}
    
    bool intersects(const Rectangle& other) const {
        return !(x + width < other.x || 
                 x > other.x + other.width ||
                 y + height < other.y || 
                 y > other.y + other.height);
    }
    
    SDL_Rect toSDLRect() const {
        return SDL_Rect{
            static_cast<int>(x),
            static_cast<int>(y),
            static_cast<int>(width),
            static_cast<int>(height)
        };
    }
};

} // namespace ENGAIN
