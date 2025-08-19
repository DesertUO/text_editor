#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "utils.h"
#include "TextBuffer.h"

#define CHAR_WIDTH 20
#define CHAR_HEIGHT 30

class TextEditor {
    private:
        bool running;

        SDL_Window* _window;
        SDL_Renderer* _renderer;

        TTF_Font* inputFont;
        std::vector<SDL_Texture*> _lineTextures;
        std::vector<SDL_FPoint> _lineSizes;

        int INPUT_WIDTH = 80;
        int INPUT_HEIGHT = 30;
        int INPUT_NUMBER_WIDTH = 50;
        int MARGIN_LEFT = 50;
        int MARGIN_TOP = 50;

        TextBuffer* textBuffer;

        int cameraTopLeftPos[2];

        int cursorPosition[2];
        int previousCursorPosition[2];
        int cursorPreservingPosition[2];

        std::unordered_map<std::string, Timer> timers;
        std::unordered_map<std::string, bool> states;

        bool _dirty = true;

        bool newFile = true;
        std::string pathToFileOpened;
    public:
        TextEditor();
        ~TextEditor();

        void init();

        void run();
        void stop();

        void handleEvents(SDL_Event event);
        void handleKeyDownEvent(const SDL_Event& e);
        void handleKeyUpEvent(const SDL_Event& e);

        void updateCamera();

        void update(float dt);
        void render();

        void renderTextBufferLineNumbers();
        void renderTextBuffer();

        void renderUIElements();

        void inputToTextBuffer(const char* input);
        void getPositionInputField(int x, int y, float& destX, float& destY);
        void getCursorPosition(float& x, float& y);

        void handleInputReturnKey();
        void handleInputBackspaceKey();
        void handleInputLeftKey();
        void handleInputRightKey();
        void handleInputUpKey();
        void handleInputDownKey();
        void handleInputTabKey();

        void handleInputLCTRLKeyRelease();

        void syncPreservingCursorPosition();
        void adjustCameraToCursor();

        void handleTextBufferSaveToFile();

};

