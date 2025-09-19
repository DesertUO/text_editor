#pragma once
#include "TextBuffer.h"

struct SaveDialogData {
    std::string* buffer;
    std::string* pathToFileOpened;

    SaveDialogData(std::string* buf, std::string& path) : buffer(buf), pathToFileOpened(&path) {}
};

struct OpenDialogData {
    TextBuffer* buffer;
    std::string* pathToFileOpened;

    OpenDialogData(TextBuffer* buf, std::string& path) : buffer(buf), pathToFileOpened(&path) {}
};
