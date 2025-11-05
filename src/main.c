#include "front.h"

int main() {

    struct dataToShow* data = calloc(1, sizeof(struct dataToShow));
    //initialize the frontend
    data->fonts = initFrontend();

    //get the music
    struct directory* OGDir = getMusic("Mp3", data->info[TAGS]);
    data->dir = OGDir;

    //back button stuff
    struct list* backList = list_create();

    //run the frontend
    while (!WindowShouldClose()) {
        mainFrontend(data, backList);

        if (data->info[BACK]) {

            //if the back button got pushed
            data->dir = list_pop(backList);
            data->info[BACK] = 0;
        } else if (data->info[TAGS]) {

            //add tags
            //bin stuff
            list_free(backList);
            mainFrontend(data, backList);
            freeDirectory(OGDir);
            //get stuff back
            OGDir = getMusic("Mp3", data->info[TAGS]);
            data->dir = OGDir;
            backList = list_create();
            data->info[TAGS] = 0;
            mainFrontend(data, backList);
        } else if (data->info[FINDDUPS]) {

            //find duplicates
            //bin stuff
            list_free(backList);
            mainFrontend(data, backList);
            freeDirectory(OGDir);
            //get stuff back
            OGDir = getMusic("Mp3", data->info[TAGS]);
            data->dir = OGDir;
            backList = list_create();
            list_insert(backList, data->dir);
            data->info[FINDDUPS] = 0;
            //do stuff
            data->duplicates = findDuplicates(data->dir);
            data->info[SHOWDUPS] = 1;
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