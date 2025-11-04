#ifndef __FRONT_H
#define __FRONT_H
#include "clay.h"
#include "globals.h"
#include "raylib.h"
#include "data.h"
#include "duplicates.h"
#include <stdlib.h>
#include <stdio.h>

struct dataToShow {
    Font* fonts;
    // struct directory* dir;
    struct clayDirectory* dir;
    int info[4];
    struct dynarray* duplicates;
};

struct clayDirectory {
    Clay_String* dirPath;
    Clay_String* dirName;
    struct list* directories;
    struct dynarray* songs;
};

struct claySong {
    Clay_String* title;
    Clay_String* artist;
};

/*-------------------------------------------------------------------------------------------------
    main functions
-------------------------------------------------------------------------------------------------*/

void freeFonts(Font* fonts);
struct dataToShow* mainFrontend(struct dataToShow* data, struct list* backList);
int windowShouldUpdate();
Font* initFrontend();

#endif