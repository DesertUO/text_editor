#include "mainEditor.h"



int main() {
    TextEditor* tEditor = new TextEditor();

    tEditor->init();
    tEditor->run();

    delete tEditor;
    return 0;
}

