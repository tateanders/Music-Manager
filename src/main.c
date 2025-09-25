#include "front.h"

int main() {
    //initialize the frontend
    Font* fonts = initFrontend();
    //get the music
    struct directory* dir = getMusic("Mp3", 0);
    //back button stuff
    struct list* backList = list_create();
    int goBack = 0;
    //run the frontend
    mainFrontend(fonts, dir, goBack);
    while (!WindowShouldClose()) {
        struct directory* newDir = mainFrontend(fonts, dir, goBack);
        if (!newDir) {
            dir = list_pop(backList);
            if (list_getNumElements(backList) == 0) {
                goBack = 0;
            }
        } else if (newDir != dir) {
            list_insert(backList, dir);
            dir = newDir;
            goBack = 1;
        }
    }

    //prevent memory leaks
    freeFonts(fonts);
    freeDirectory(dir);
    return 0;
}