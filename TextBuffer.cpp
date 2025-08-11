#include "TextBuffer.h"

TextBuffer::TextBuffer(const std::string&) {
    buffer = {{}};
}

TextBuffer::~TextBuffer() {
    freeBuffer();
}

void TextBuffer::freeBuffer() {
    for(int i = 0; i < buffer.size(); i++) {
        auto& line = buffer[i];
        for(int j = 0; j < line.size(); j++) {
            delete[] line[j].charPtr;
            line.erase(line.begin() + j);
        }
        buffer.erase(buffer.begin() + i);
    }
}

void TextBuffer::loadText(std::string text) {
    std::vector<std::vector<CharBuff>> newBuffer = {{}};
    int currentLine = 0;
    size_t textSize = text.size();
    for(size_t i = 0; i < textSize; i++) {
        char* newChar = extractUtf8Char(text, i);

        if(i < textSize && text[i] == '\n') {
            newBuffer.emplace_back();
            currentLine++;
            continue;
        }
        newBuffer[currentLine].emplace_back(newChar);
    }

    buffer = newBuffer;
}

std::string TextBuffer::bufferToString() {
    std::string str = "";
    size_t textBufferSize = buffer.size();
    for(size_t i = 0; i < textBufferSize; i++) {
        for(const auto& c: buffer[i]) {
            str += c.charPtr;
        }
        if(i + 1 < textBufferSize)
            str += '\n';
    }
    return str;
}

Uint32 TextBuffer::sizeOfTextBufferBytes() {
    Uint32 totalBytes = 0;
    /* textBuffer: std::vector<std::vector<char*>> */
    totalBytes += buffer.capacity() + sizeof(std::vector<char*>);
    for(const auto& line : buffer) {
        totalBytes += line.capacity() * sizeof(char*);
        for(const auto& c: line) {
            if(c.charPtr != nullptr) {
                totalBytes += strlen(c.charPtr) + 1;
            }
        }
    }

    return totalBytes;
}

const std::vector<std::vector<CharBuff>>& TextBuffer::getBuffer() {
    return buffer;
}

std::vector<std::vector<CharBuff>>& TextBuffer::getBufferModifiable() {
    return buffer;
}

