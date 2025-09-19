#pragma once
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>
#include <SDL3/SDL.h>

struct Vec2 {
    float x;
    float y;
};

struct Vec2Int {
    int x;
    int y;
};

typedef std::vector<uint8_t> grapheme;

struct Timer {
    float value = 0.0f;
    std::function<float(float)> updateFunction;
};

Uint32 char_to_uint32(const char* input);

std::vector<Uint32> int_to_uint32_digits(int number);

std::vector<Uint32> int_to_uint32_digits_rev(int number);

Uint32 string_to_uint32(const char* s);

int ordersOfMagnitudeApart(int a, int b);

int countDigits(int n);

char* extractUtf8Char(const std::string& text, size_t i);

