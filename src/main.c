#include "front.h"

int main() {
    //initialize the frontend
    Font* fonts = initFrontend();
    //get the music
    struct directory* music = getMusic("Mp3", 0);
    mainFrontend(fonts, music);
    while (!WindowShouldClose()) {
        struct directory* newMusic = mainFrontend(fonts, music);
        
    }

    //prevent memory leaks
    freeFonts(fonts);
    freeDirectory(music);
    return 0;
}