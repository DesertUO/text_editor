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
            line.erase(line.begin() + j);
        }
        buffer.erase(buffer.begin() + i);
    }
}

void TextBuffer::loadText(const std::string& text) {
    std::vector<std::vector<CharBuff>> newBuffer = {{}};
    int currentLine = 0;
    size_t textSize = text.length();
    for(size_t i = 0; i < textSize; i++) {
        if(i < (textSize-1) && text[i] == '\n') {
            newBuffer.emplace_back();
            currentLine++;
            continue;
        }
        if(i == (textSize-1) && text[i] == '\n') {
            break;
        }
        char* newChar = extractUtf8Char(text, i);
        newBuffer[currentLine].emplace_back(newChar);
    }

    buffer = newBuffer;
}

std::string TextBuffer::bufferToString() {
    std::string str = "";
    size_t textBufferSize = buffer.size();
    for(size_t i = 0; i < textBufferSize; i++) {
        // Append all characters from the i-th line
        for(const auto& c: buffer[i]) {
            SDL_Log("Current char: %s", c.ch.c_str());
            str += c.ch;
        }
        // If there's another line, add newline character
        if(i + 1 <= textBufferSize)
            str += std::string("\n");
    }
    SDL_Log("Final text from buffer: %s", str.c_str());
    return str;
}

TextBuffer TextBuffer::stringToBuffer(const std::string& text) {
    TextBuffer tb("");
    tb.loadText(text);
    return tb;
}

Uint32 TextBuffer::sizeBytes() {
    Uint32 totalBytes = 0;
    /* textBuffer: std::vector<std::vector<std::string>> */
    totalBytes += buffer.capacity() * sizeof(line) + sizeof(std::vector<char*>);
    for(const auto& line : buffer) {
        totalBytes += line.capacity() * sizeof(CharBuff);
        for(const auto& c: line) {
            if(!c.ch.empty()) {
                totalBytes += c.ch.size();;
            }
        }
    }

    return totalBytes;
}

const std::vector<std::vector<CharBuff>>& TextBuffer::getBuffer() {
                // To do: handleTextBufferLoadFromFile();
    return buffer;
}

std::vector<std::vector<CharBuff>>& TextBuffer::getBufferModifiable() {
    return buffer;
}

