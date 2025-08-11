#pragma once
#include <vector>
#include <string>
// #include <SDL3/
#include "utils.h"

struct CharBuff {
    char* charPtr;
};

class TextBuffer {
    private:
        std::vector<std::vector<CharBuff>> buffer;
    public:
        TextBuffer(const std::string& = "");
        ~TextBuffer();

        const std::vector<std::vector<CharBuff>>& getBuffer();
        std::vector<std::vector<CharBuff>>& getBufferModifiable();

        void loadText(std::string text);
        std::string bufferToString();

        void freeBuffer();

        Uint32 sizeOfTextBufferBytes();
};
