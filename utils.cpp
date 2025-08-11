#include "utils.h"


Uint32 char_to_uint32(const char* input) {
    Uint32 result = 0;

    // Copy up to 4 bytes from input into result
    for (int i = 0; i < 4 && input[i] != '\0'; i++) {
        result |= ((Uint32)(unsigned char)input[i]) << (8 * i);
    }

    return result;
}

std::vector<Uint32> int_to_uint32_digits(int number) {
    std::vector<Uint32> result;
    std::string str = std::to_string(number);

    for (char c : str) {
        Uint32 val = (Uint32)(unsigned char)c;
        result.push_back(val);
    }

    return result;
}

std::vector<Uint32> int_to_uint32_digits_rev(int number) {
    std::vector<Uint32> result;
    std::string str = std::to_string(number);
    std::reverse(str.begin(), str.end());

    for (char c : str) {
        Uint32 val = (Uint32)(unsigned char)c;
        result.push_back(val);
    }

    return result;
}

int ordersOfMagnitudeApart(int a, int b) {
    if (a == 0) {
        return 0;
    }
    double ratio = static_cast<double>(b) / a;
    if (ratio <= 0) {
        return 0;
    }
    return static_cast<int>(std::floor(std::log10(ratio)));
}

int countDigits(int number) {
    number = std::abs(number);
    if(number == 0) {
        return 0;
    }
    int d = 0;
    while(number > 0) {
        number = number / 10;
        d++;
    }
    return d;
}

int utf8CharLength(unsigned char c) {
    if ((c & 0x80) == 0)        return 1; // 0xxxxxxx
    else if ((c & 0xE0) == 0xC0) return 2; // 110xxxxx
    else if ((c & 0xF0) == 0xE0) return 3; // 1110xxxx
    else if ((c & 0xF8) == 0xF0) return 4; // 11110xxx
    else return 1; // invalid byte, fallback
}

char* extractUtf8Char(const std::string& text, size_t i) {
    unsigned char firstByte = text[i];
    int len = utf8CharLength(firstByte);

    if (i + len > text.size()) {
        len = text.size() - i;
    }

    char* utf8Char = new char[len + 1];

    for (int j = 0; j < len; j++) {
        utf8Char[j] = text[i + j];
    }
    utf8Char[len] = '\0';

    return utf8Char;
}
