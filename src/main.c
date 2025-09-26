#include "front.h"
#include <pthread.h>

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
    while (!WindowShouldClose()) {
        //run frontend
        struct directory* newDir = mainFrontend(fonts, dir, goBack, tagsAdded);
        if (newDir == 0) {
            //if the back button got pushed
            dir = list_pop(backList);
            if (list_getNumElements(backList) == 0) {
                goBack = 0;
            }
        } else if (*tagsAdded) {
            //if the add metadata button got pushed
            mainFrontend(fonts, NULL, goBack, tagsAdded);
            freeDirectory(OGDir);
            list_free(backList);
            backList = list_create();
            OGDir = getMusic("Mp3", 1);
            dir = OGDir;
            *tagsAdded = 0;
            goBack = 0;
            mainFrontend(fonts, OGDir, goBack, tagsAdded);
        } else if (newDir != dir) {
            //if a directory button got pushed
            list_insert(backList, dir);
            dir = newDir;
            goBack = 1;
        }
    }

    //prevent memory leaks
    list_free(backList);
    freeFonts(fonts);
    freeDirectory(OGDir);
    return 0;
}