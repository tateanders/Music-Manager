#include "front.h"
#include <pthread.h>

int main() {
    struct dataToShow* data = calloc(1, sizeof(struct dataToShow));
    //initialize the frontend
    data->fonts = initFrontend();

    //get the music
    data->dir = getMusic("Mp3", 0);

    //back button stuff
    struct list* backList = list_create();

    //run the frontend
    while (!WindowShouldClose()) {
        //run frontend
        mainFrontend(data, backList);
        if (data->backPushed) {
            //if the back button got pushed
            data->dir = list_pop(backList);
            data->backPushed = 0;
        } else if (data->dirPushed) {
            //if a directory button got pushed
            data->dirPushed = 0;
        }
        else if (data->tagsAdded) {
            //if the add metadata button got pushed
            mainFrontend(data, backList);
            freeDirectory(data->dir);
            list_free(backList);
            data->dir = getMusic("Mp3", 1);
            backList = list_create();
            data->tagsAdded = 0;
            mainFrontend(data, backList);
        }

    }

    //prevent memory leaks
    list_free(backList);
    freeFonts(data->fonts);
    freeDirectory(data->dir);
    free(data);
    return 0;
}