#include "mainEditor.h"

/*
 * Constructor
 * */
TextEditor::TextEditor() {
    running = false;
    _window = nullptr;
    _renderer = nullptr;

    inputFont = nullptr;

    // Input initialization
    textBuffer = new TextBuffer();

    cameraTopLeftPos[0] = 0;
    cameraTopLeftPos[1] = 0;

    cursorPosition[0] = 0;
    cursorPosition[1] = 0;

    previousCursorPosition[0] = 0;
    previousCursorPosition[1] = 0;

    cursorPreservingPosition[0] = 0;
    cursorPreservingPosition[1] = 0;

    timers = {};
    togglers = {};
}

/*
 * Destructor
 * */
TextEditor::~TextEditor() {
    textBuffer->freeBuffer();
    delete textBuffer;

    for (auto* tx : _lineTextures) SDL_DestroyTexture(tx);
    if (inputFont) TTF_CloseFont(inputFont);
        TTF_Quit();

    SDL_DestroyRenderer(_renderer);
    SDL_DestroyWindow(_window);
}



void TextEditor::init() {
    if(SDL_Init(SDL_INIT_VIDEO) == false) {
        SDL_Log("Error while initializing SDL3: %s", SDL_GetError());
        SDL_Quit();
        return;
    }

    _window = SDL_CreateWindow("Text Editor", 1925, 1080, SDL_WINDOW_RESIZABLE);
    if(!_window) {
        SDL_Log("Error while creating window: %s", SDL_GetError());
        SDL_Quit();
        return;
    }

    _renderer = SDL_CreateRenderer(_window, NULL);
    if(!_renderer) {
        SDL_Log("Error while creating renderer: %s", SDL_GetError());
        SDL_Quit();
        return;
    }

    /*
     * Different relative paths to executable
     * */
    // Base path
    const char* basePath = SDL_GetBasePath();
    if(!basePath) {
        SDL_Log("SDL_GetBasePath failed: %s", SDL_GetError());
        return;
    }
    std::string basePathStr(basePath);
    delete basePath;

    std::string texturePath = basePathStr + "src/textures/";
    std::string fontPath = basePathStr + "src/fonts/";


    if(TTF_Init() == false) {
        SDL_Log("Error while initializing TTF library");
        TTF_Quit();
        SDL_Quit();
        return;
    }

    inputFont = TTF_OpenFont((fontPath + "JetBrainsMonoNerdFont_Regular.ttf").c_str(), CHAR_WIDTH);
    if(!inputFont) {
        SDL_Log("Error while loading fonts: %s", SDL_GetError());
        return;
    }
}

void TextEditor::run() {
    running = true;
    SDL_StartTextInput(_window);

    timers["cursor_blink"] = {
        0.0f,
        [](float current) {
            return (current > 1000000.0f) ? -1000000.0f : current;
        }
    };

    // Give me the structure of how should I implement the update/render's dt for animation purposes
    // Based on a always changing frame rate
    float dt = 0.0f;
    Uint64 lastTime = SDL_GetTicksNS();

    while(running) {
        Uint64 currentTime = SDL_GetTicksNS();
        dt = (currentTime - lastTime) / 1000.0f; // Convert to seconds
        lastTime = currentTime;
        update(dt);
        render();
    }

    SDL_StopTextInput(_window);
}

void TextEditor::stop() {
    running = false;
}

void TextEditor::inputToTextBuffer(const char* input) {
    char* dest = new char[strlen(input) + 1];
    strcpy(dest, input);
    CharBuff newDest;
    newDest.charPtr = dest;

    std::vector<CharBuff>& currentLine = textBuffer->getBufferModifiable()[cursorPosition[1]];

    currentLine.insert(currentLine.begin() + cursorPosition[0], newDest);
    cursorPosition[0]++;

    int currentCharsInLine = currentLine.size();
    int relativeX = currentCharsInLine - cameraTopLeftPos[0];

    if(relativeX > (INPUT_WIDTH) && cursorPosition[0] > INPUT_WIDTH) {
        cameraTopLeftPos[0]++;
    }

    cursorPreservingPosition[0] = cursorPosition[0];
}

// Handle adding a new line
void TextEditor::handleInputReturnKey() {
    int currentLineSize = textBuffer->getBuffer()[cursorPosition[1]].size();
    if(textBuffer->getBuffer().size() > (INPUT_HEIGHT - 1)) {
        cameraTopLeftPos[1]++;
    }

    textBuffer->getBufferModifiable().insert(textBuffer->getBuffer().begin() + cursorPosition[1] + 1, std::vector<CharBuff>{});

    cursorPosition[1]++;
    cursorPreservingPosition[1] = cursorPosition[1];

    if(cursorPosition[0] < (currentLineSize)) {
        std::vector<CharBuff>* prevLine = &textBuffer->getBufferModifiable()[cursorPosition[1] - 1];
        std::vector<CharBuff>* destLine = &textBuffer->getBufferModifiable()[cursorPosition[1]];
        destLine->insert(
            destLine->end(),
            std::make_move_iterator(prevLine->begin() + cursorPosition[0]),
            std::make_move_iterator(prevLine->end())
        );
        prevLine->erase(prevLine->begin() + cursorPosition[0], prevLine->end());
    }

    cursorPosition[0] = 0;
    cameraTopLeftPos[0] = 0;
    cursorPreservingPosition[0] = 0;
}

void TextEditor::handleInputBackspaceKey() {
    // Delete on line
    std::vector<CharBuff>* previousLine = &textBuffer->getBufferModifiable()[cursorPosition[1]-1];
    if(cursorPosition[0] > 0) {
        std::vector<CharBuff>* currentLine = &textBuffer->getBufferModifiable()[cursorPosition[1]];

        char * toDelete = (*currentLine)[cursorPosition[0] - 1].charPtr;
        delete[] toDelete;

        currentLine->erase(currentLine->begin() + cursorPosition[0] - 1);
        cursorPosition[0]--;
    }
    else if(cursorPosition[0] == 0) {

        if(cursorPosition[1] <= 0) { return; }

        if(textBuffer->getBuffer()[cursorPosition[1]].size() > 0) {
            std::vector<CharBuff>* prevLine = &textBuffer->getBufferModifiable()[cursorPosition[1]];
            std::vector<CharBuff>* destLine = &textBuffer->getBufferModifiable()[cursorPosition[1] - 1];
            int prevDestLineSize = destLine->size();
            destLine->insert(
                destLine->end(),
                std::make_move_iterator(prevLine->begin() + cursorPosition[0]),
                std::make_move_iterator(prevLine->end())
            );
            prevLine->erase(prevLine->begin() + cursorPosition[0], prevLine->end());

            cursorPosition[0] = prevDestLineSize;
        } else {
            cursorPosition[0] = previousLine->size();
        }
        textBuffer->getBufferModifiable().erase(textBuffer->getBuffer().begin() + cursorPosition[1]);

        cursorPosition[1]--;
    }

    cursorPreservingPosition[0] = cursorPosition[0];
    cursorPreservingPosition[1] = cursorPosition[1];
}

void TextEditor::handleInputLeftKey() {
    if(cursorPosition[0] <= 0) {
        int currentLinePos = cursorPosition[1];
        if(currentLinePos <= 0) { return; }

        int prevLineSize = textBuffer->getBuffer()[cursorPosition[1]-1].size();

        cursorPosition[1]--;
        cursorPreservingPosition[1] = cursorPosition[1];

        cursorPosition[0] = prevLineSize + 1;
        cursorPreservingPosition[0] = cursorPosition[0];
    }

    int relativeX = cursorPosition[0] - cameraTopLeftPos[0];
    int relativeY = cursorPosition[1] - cameraTopLeftPos[1];

    if(relativeX <= 0) {
        if(cameraTopLeftPos[0] <= 0) { return; }
        cameraTopLeftPos[0]--;
        return;
    }

    if(relativeY < 0) {
        cameraTopLeftPos[1]--;
    }

    cursorPosition[0]--;
    cursorPreservingPosition[0] = cursorPosition[0];
}

void TextEditor::handleInputRightKey() {
    int lineSize = textBuffer->getBuffer()[cursorPosition[1]].size();

    int relativeX = cursorPosition[0] - cameraTopLeftPos[0];
    int relativeY = cursorPosition[1] - cameraTopLeftPos[1];

    if(lineSize < 0) { return; }

    if(cursorPosition[0] >= lineSize) {
        int currentLine = cursorPosition[1];
        int textBufferSize = textBuffer->getBuffer().size();
        if(currentLine >= (textBufferSize - 1)) { return; }

        // To "zero" 'cause we dont break here :(
        cursorPosition[0] = -1;
        cursorPreservingPosition[0] = -1;

        cursorPosition[1]++;
        cursorPreservingPosition[1] = cursorPosition[1];

        cameraTopLeftPos[0] = 0;
    }

    relativeX = cursorPosition[0] - cameraTopLeftPos[0];
    relativeY = cursorPosition[1] - cameraTopLeftPos[1];

    if(relativeX >= INPUT_WIDTH) {
        cameraTopLeftPos[0]++;
    }

    if(relativeY >= INPUT_HEIGHT) {
        cameraTopLeftPos[1]++;
    }

    cursorPosition[0]++;
    cursorPreservingPosition[0] = cursorPosition[0];
}

void TextEditor::handleInputUpKey() {
    if(cursorPosition[1] <= 0) { return; }
    cursorPosition[1]--;

    int relativeX = cursorPosition[0] - cameraTopLeftPos[0];
    int relativeY = cursorPosition[1] - cameraTopLeftPos[1];

    if(relativeY < 0) {
        cameraTopLeftPos[1]--;
    }


    int newLineSize = textBuffer->getBuffer()[cursorPosition[1]].size();
    if(newLineSize < cursorPreservingPosition[0]) {
        cursorPosition[0] = newLineSize;
    } else {
        cursorPosition[0] = cursorPreservingPosition[0];
    }
}

void TextEditor::handleInputDownKey() {
    int textLinesBufferSize = textBuffer->getBuffer().size();
    if(textLinesBufferSize <= 0) { return; }
    if(cursorPosition[1] >= (textLinesBufferSize - 1)) { return; }
    cursorPosition[1]++;

    int relativeY = cursorPosition[1] - cameraTopLeftPos[1];
    if(relativeY >= INPUT_HEIGHT) {
        cameraTopLeftPos[1]++;
    }

    int newLineSize = textBuffer->getBuffer()[cursorPosition[1]].size();
    if(newLineSize < cursorPreservingPosition[0]) {
        cursorPosition[0] = newLineSize;
    } else {
        cursorPosition[0] = cursorPreservingPosition[0];
    }
}

void TextEditor::handleInputTabKey() {
    int cursorX = cursorPosition[0];
    int cursorY = cursorPosition[1];
    std::vector<CharBuff>* currentLine = &textBuffer->getBufferModifiable()[cursorY];
    for(int i = 0; i < 4; i++) {
        char space[2] = " ";
        char* newSpace = extractUtf8Char(space, 0);
        CharBuff spaceBuff;
        spaceBuff.charPtr = newSpace;
        currentLine->insert(currentLine->begin() + cursorX, spaceBuff);
    }

    cursorPosition[0] = cursorPosition[0] + 4;
    cursorPreservingPosition[0] = cursorPosition[0];
    int relativeX = cursorPosition[0] - cameraTopLeftPos[1];
    if(relativeX > INPUT_WIDTH) { cameraTopLeftPos[0] += 4; }
}

void TextEditor::handleKeyDownEvent(const SDL_Event& e) {
    switch(e.key.key) {
        case SDLK_RETURN:
            handleInputReturnKey();
            break;

        case SDLK_BACKSPACE:
            handleInputBackspaceKey();
            break;

        case SDLK_LEFT:
            handleInputLeftKey();
            break;

        case SDLK_RIGHT:
            handleInputRightKey();
            break;

        case SDLK_UP:
            handleInputUpKey();
            break;

        case SDLK_DOWN:
            handleInputDownKey();
            break;

        case SDLK_TAB:
            handleInputTabKey();
            break;
    }
}

void TextEditor::handleKeyUpEvent(const SDL_Event& e) {}

void TextEditor::handleEvents(SDL_Event e) {
    while(SDL_PollEvent(&e)) {
        switch(e.type) {
            case SDL_EVENT_QUIT:
                stop();
                break;
            case SDL_EVENT_TEXT_INPUT: {
                auto inputChar = e.text.text;
                inputToTextBuffer(inputChar);
                break;
            }
            case SDL_EVENT_KEY_DOWN:
                handleKeyDownEvent(e);
                break;
            case SDL_EVENT_KEY_UP: {
                break;
            }
            default:
                break;
        }
    }
}


void TextEditor::update(float dt) {
    SDL_Event event;
    handleEvents(event);

    if(textBuffer->getBuffer().size() > 0)
        INPUT_NUMBER_WIDTH =  countDigits(textBuffer->getBuffer().size()) * CHAR_WIDTH;

    for(auto &timer: timers) {
        timer.second.value += dt;
        timer.second.value = timer.second.updateFunction(timer.second.value);
    }

    updateCamera();
}


void TextEditor::getPositionInputField(int x, int y, float& destX, float& destY) {
    destX = x*CHAR_WIDTH + INPUT_NUMBER_WIDTH + MARGIN_LEFT;
    destY = y*CHAR_HEIGHT + MARGIN_TOP;
}

void TextEditor::getCursorPosition(float& x, float& y) {
    int relativeX = cursorPosition[0] - cameraTopLeftPos[0];
    int relativeY = cursorPosition[1] - cameraTopLeftPos[1];
    x = relativeX * CHAR_WIDTH + INPUT_NUMBER_WIDTH + MARGIN_LEFT;
    y = relativeY * CHAR_HEIGHT + MARGIN_TOP;
}

void TextEditor::updateCamera() {
    int cursorX = cursorPosition[0];
    int cursorY = cursorPosition[1];
    int relativeX = cursorX - cameraTopLeftPos[0];
    int relativeY = cursorY - cameraTopLeftPos[1];

    int HALF_INPUT_WIDTH = (int)(INPUT_WIDTH / 2.0f);
    int HALF_INPUT_HEIGHT = (int)(INPUT_HEIGHT / 2.0f);

    if(relativeX < 0) {
        if (cursorX < INPUT_WIDTH) { cameraTopLeftPos[0] = 0; }
        else { cameraTopLeftPos[0] = cursorX - HALF_INPUT_WIDTH; }
    } else if (relativeX > INPUT_WIDTH) {
        cameraTopLeftPos[0] = cursorX - HALF_INPUT_WIDTH;
    }

    if(relativeY < 0) {
        if(cursorY < INPUT_HEIGHT) { cameraTopLeftPos[1] = 0; }
        else { cameraTopLeftPos[1] = cursorY - HALF_INPUT_HEIGHT; }
    } else if(relativeY > INPUT_HEIGHT) {
        cameraTopLeftPos[1] = cursorY - HALF_INPUT_HEIGHT;
    }
}

void TextEditor::renderTextBufferLineNumbers() {
    float cameraTopPos, cameraLeftPos;

    getPositionInputField(cameraTopLeftPos[0], cameraTopLeftPos[1], cameraTopPos, cameraLeftPos);

    SDL_Color currentDrawColor;
    SDL_GetRenderDrawColor(_renderer, &currentDrawColor.r, &currentDrawColor.g, &currentDrawColor.b, &currentDrawColor.a);

    int y = 0;
    for(auto line: textBuffer->getBuffer()) {
        int relativeY = y - cameraTopLeftPos[1];

        if(relativeY < 0 | relativeY > (INPUT_HEIGHT - 1)) { y++; continue; }

        int x = -1;
        std::vector<Uint32> characters = int_to_uint32_digits_rev(y + 1);
        for(auto c: characters) {

            // Render character surface
            SDL_Surface* textSurface = TTF_RenderGlyph_Blended(inputFont, c, currentDrawColor);
            if (!textSurface) {
                SDL_Log("Text rendering failed: %s", SDL_GetError());
                continue;
            }

            // Convert to texture
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(_renderer, textSurface);
            if (!textTexture) {
                SDL_Log("Texture creation failed: %s", SDL_GetError());
                SDL_DestroySurface(textSurface);
                continue;
            }

            float rX, rY;

            int relativeX = x;
            int relativeY = y - cameraTopLeftPos[1];

            getPositionInputField(relativeX, relativeY, rX, rY);

            SDL_FRect dst = { rX, rY, static_cast<float>(textSurface->w), static_cast<float>(textSurface->h) };
            SDL_RenderTexture(_renderer, textTexture, NULL, &dst);

            SDL_DestroyTexture(textTexture);
            SDL_DestroySurface(textSurface);
            x--;
        }
        y++;
    }
}

void TextEditor::renderTextBuffer() {
    float cameraTopPos, cameraLeftPos;

    getPositionInputField(cameraTopLeftPos[0], cameraTopLeftPos[1], cameraTopPos, cameraLeftPos);

    SDL_Color currentDrawColor;
    SDL_GetRenderDrawColor(_renderer, &currentDrawColor.r, &currentDrawColor.g, &currentDrawColor.b, &currentDrawColor.a);

    int y = 0;
    for(auto line: textBuffer->getBuffer()) {
        int relativeY = y - cameraTopLeftPos[1];

        if(relativeY < 0 | relativeY > (INPUT_HEIGHT + 1)) { y++; continue; }

        int x = 0;
        for(auto c: line) {
            int relativeX = x - cameraTopLeftPos[0];

            if(relativeX < 0 | relativeX > (INPUT_WIDTH + 1)) { x++; continue; }
            // In which c is a char* that represents a grapheme cluster: UTF-8 encoded

            // Render character surface
            SDL_Surface* textSurface = TTF_RenderText_Blended(inputFont, c.charPtr, 0, currentDrawColor);
            if (!textSurface) {
                SDL_Log("Text rendering failed: %s", SDL_GetError());
                continue;
            }

            // Convert to texture
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(_renderer, textSurface);
            if (!textTexture) {
                SDL_Log("Texture creation failed: %s", SDL_GetError());
                SDL_DestroySurface(textSurface);
                continue;
            }

            float rX, rY;

            getPositionInputField(relativeX, relativeY, rX, rY);


            SDL_FRect dst = { rX, rY, static_cast<float>(textSurface->w), static_cast<float>(textSurface->h) };
            SDL_RenderTexture(_renderer, textTexture, NULL, &dst);

            SDL_DestroyTexture(textTexture);
            SDL_DestroySurface(textSurface);
            x++;
        }
        y++;
    }
}

void TextEditor::render() {
    int winWidth, winHeight;
    SDL_GetWindowSize(_window, &winWidth, &winHeight);

    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(_renderer);

    // Render the editor background
    float desiredWidth = (float)INPUT_WIDTH * CHAR_WIDTH;
    float desiredHeight = (float)INPUT_HEIGHT * CHAR_HEIGHT;

    SDL_FRect inputBox = {
        (float)INPUT_NUMBER_WIDTH + MARGIN_LEFT, (float)MARGIN_TOP,
        desiredWidth, desiredHeight
    };

    float numberBoxWidth = (float)INPUT_NUMBER_WIDTH;
    SDL_FRect numberBox = {
        (float) MARGIN_LEFT, (float) MARGIN_TOP,
        numberBoxWidth, desiredHeight
    };

    // Input box
    SDL_SetRenderDrawColor(_renderer, 240, 240, 240, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(_renderer, &inputBox);

    // Render the cursor based on a timer
    if(timers["cursor_blink"].value > 0.0f) {
        float cursorX, cursorY;
        getCursorPosition(cursorX, cursorY);
        SDL_FRect cursorRect = { cursorX, cursorY, CHAR_WIDTH, CHAR_HEIGHT };
        SDL_SetRenderDrawColor(_renderer, 200, 200, 200, SDL_ALPHA_OPAQUE);
        SDL_RenderFillRect(_renderer, &cursorRect);
    }

    // Render the actual editor content
    SDL_SetRenderDrawColor(_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    renderTextBuffer();

    // Number lines box
    SDL_SetRenderDrawColor(_renderer, 220, 220, 220, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(_renderer, &numberBox);
    SDL_SetRenderDrawColor(_renderer, 100, 100, 200, SDL_ALPHA_OPAQUE);
    renderTextBufferLineNumbers();

    // Render the container like the wrapper of the editor for
    // "aesthetics" reasons
    SDL_SetRenderDrawColor(_renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);

    SDL_RenderPresent(_renderer);
}

