#include "front.h"

int main() {
    struct dataToShow* data = calloc(1, sizeof(struct dataToShow));
    //initialize the frontend
    data->fonts = initFrontend();

    //get the music
    struct directory* OGDir = getMusic("Mp3", 0);
    data->dir = OGDir;

    //back button stuff
    struct list* backList = list_create();

    //run the frontend
    while (!WindowShouldClose()) {
        mainFrontend(data, backList);
        if (data->backPushed) {
            //if the back button got pushed
            data->dir = list_pop(backList);
            data->backPushed = 0;
        } else if (data->tagsAdded) {
            //add tags
            //bin stuff
            list_free(backList);
            mainFrontend(data, backList);
            freeDirectory(OGDir);
            //get stuff back
            OGDir = getMusic("Mp3", 1);
            data->dir = OGDir;
            backList = list_create();
            data->tagsAdded = 0;
            mainFrontend(data, backList);
        } else if (data->findDups) {
            //find duplicates
            //bin stuff
            list_free(backList);
            mainFrontend(data, backList);
            freeDirectory(OGDir);
            //get stuff back
            OGDir = getMusic("Mp3", 0);
            data->dir = OGDir;
            backList = list_create();
            list_insert(backList, data->dir);
            data->findDups = 0;
            //do stuff
            data->duplicates = findDuplicates(data->dir);
            data->showDuplicates = 1;
            mainFrontend(data, backList);
        }
    }

    //prevent memory leaks
    if (data->duplicates) {
        freeDups(data->duplicates);
    }
    list_free(backList);
    freeFonts(data->fonts);
    freeDirectory(OGDir);
    free(data);
    return 0;
}