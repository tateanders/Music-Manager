#include "front.h"

int main() {
    //initialize the frontend
    Font* fonts = initFrontend();
    //get the music
    struct directory* OGDir = getMusic("Mp3", 0);
    struct directory* dir = OGDir;
    //back button stuff
    struct list* backList = list_create();
    int goBack = 0;
    //tag button stuff
    int addedTags = 0;
    int* tagsAdded = &addedTags;
    //run the frontend
    mainFrontend(fonts, dir, goBack, tagsAdded);
    while (!WindowShouldClose()) {
        struct directory* newDir = mainFrontend(fonts, dir, goBack, tagsAdded);
        if (newDir == 0) {
            dir = list_pop(backList);
            if (list_getNumElements(backList) == 0) {
                goBack = 0;
            }
        } else if (*tagsAdded == 500) {
            list_free(backList);
            freeDirectory(OGDir);
            OGDir = getMusic("Mp3", 1);
            dir = OGDir;
        } else if (newDir != dir) {
            list_insert(backList, dir);
            dir = newDir;
            goBack = 1;
        }
        if (*tagsAdded) {
            *tagsAdded -= 1;
        }
    }

    //prevent memory leaks
    list_free(backList);
    freeFonts(fonts);
    freeDirectory(OGDir);
    return 0;
}