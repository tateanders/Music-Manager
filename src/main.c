#include "front.h"

/*-------------------------------------------------------------------------------------------------
    make/free the clay strings
-------------------------------------------------------------------------------------------------*/

Clay_String* makeClayString(const char* string) {
    //make a copy of the string
    char* string2 = strdup(string);;
    //create the clay string
    Clay_String* clayString = (Clay_String*)calloc(1, sizeof(Clay_String));
    clayString->isStaticallyAllocated = false;
    clayString->length = (int32_t)strlen(string2);
    clayString->mutChars = string2;
    clayString->chars = string2;
    return clayString;
}

void freeClayString(Clay_String* string) {
    free(string->mutChars);
    free(string);
}

/*-------------------------------------------------------------------------------------------------
    create a directory to hold the clay strings
-------------------------------------------------------------------------------------------------*/

void freeClayDir(struct clayDirectory* dir) {
    freeClayString(dir->dirPath);
    freeClayString(dir->dirName);
    if (dir->directories) {
        while (list_getNumElements(dir->directories)) {
            struct clayDirectory* dir2 = (struct clayDirectory*)list_pop(dir->directories);
            freeClayDir(dir2);
        }
        list_free(dir->directories);
    }
    if (dir->songs) {
        int i;
        for (i = 0; i < dynarray_size(dir->songs); i++) {
            struct claySong* song = (struct claySong*)dynarray_get(dir->songs, i);
            freeClayString(song->title);
            freeClayString(song->artist);
        }
        dynarray_free(dir->songs);
    }
    free(dir);
}

struct clayDirectory* getClayDir(struct directory* dir) {
    struct clayDirectory* clayDir = (struct clayDirectory*)calloc(1, sizeof(struct clayDirectory));
    //add the dirpath
    clayDir->dirPath = makeClayString(dir->dirPath);
    clayDir->dirName = makeClayString(dir->dirName);
    //fill directories
    if (dir->directories) {
        clayDir->directories = list_create();
        int i;
        for (i = 0; i < list_getNumElements(dir->directories); i++) {
            struct directory* dir2 = (struct directory*)list_getElement(dir->directories, i);
            struct clayDirectory* clayDir2 = getClayDir(dir2);
            list_insert(clayDir->directories, (void*)clayDir2);
        }
    }
    //fill songs
    if(dir->songs) {
        clayDir->songs = dynarray_create();
        int i;
        for (i = 0; i < dynarray_size(dir->songs); i++) {
            struct song* song = (struct song*)dynarray_get(dir->songs, i);
            struct claySong* claySong = (struct claySong*)calloc(1, sizeof(struct claySong));
            claySong->title = makeClayString(song->title);
            claySong->artist = makeClayString(song->artist);
            dynarray_push(clayDir->songs, claySong);
        }
    }
    return clayDir;
}

/*-------------------------------------------------------------------------------------------------
    main
-------------------------------------------------------------------------------------------------*/

int main() {
    SetTraceLogLevel(LOG_NONE);
    struct dataToShow* data = calloc(1, sizeof(struct dataToShow));
    //initialize the frontend
    data->fonts = initFrontend();

    //get the music
    struct directory* OGDir = getMusic("Mp3", data->info[TAGS]);
    data->dir = getClayDir(OGDir);

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
            freeClayDir(data->dir);
            //get stuff back
            OGDir = getMusic("Mp3", data->info[TAGS]);
            data->dir = getClayDir(OGDir);
            backList = list_create();
            data->info[TAGS] = 0;
            mainFrontend(data, backList);

        } else if (data->info[FINDDUPS]) {

            //find duplicates
            //bin stuff
            list_free(backList);
            mainFrontend(data, backList);
            freeDirectory(OGDir);
            freeClayDir(data->dir);
            //get stuff back
            OGDir = getMusic("Mp3", data->info[TAGS]);
            data->dir = getClayDir(OGDir);
            backList = list_create();
            list_insert(backList, data->dir);
            data->info[FINDDUPS] = 0;
            //do stuff
            data->duplicates = findDuplicates(OGDir);
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
    freeClayDir(data->dir);
    free(data);
    return 0;
}