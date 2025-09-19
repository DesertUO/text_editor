#pragma once
#include <vector>
#include <string>
// #include <SDL3/
#include "utils.h"

struct CharBuff {
    std::string ch;
};

typedef std::vector<CharBuff> line;

class TextBuffer {
    private:
        std::vector<line> buffer;
    public:
        TextBuffer(const std::string& = "");
        ~TextBuffer();

        const std::vector<std::vector<CharBuff>>& getBuffer();
        std::vector<std::vector<CharBuff>>& getBufferModifiable();

        void loadText(const std::string& text);

        std::string bufferToString();
        TextBuffer stringToBuffer(const std::string& text);

        void freeBuffer();

        Uint32 sizeBytes();
};
