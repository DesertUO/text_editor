#pragma once
#include <vector>
#include <string>
// #include <SDL3/
#include "utils.h"

struct CharBuff {
    char* charPtr;
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

        void loadText(std::string text);
        std::string bufferToString();

        void freeBuffer();

        Uint32 sizeBytes();
};
