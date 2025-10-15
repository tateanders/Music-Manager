#ifndef __FRONT_H
#define __FRONT_H
#include "clay.h"
#include "globals.h"
// #include "raymath.h"
// #include "clay_renderer_raylib.c"
#include "raylib.h"
#include "data.h"
#include <stdlib.h>
#include <stdio.h>

struct dataToShow {
    Font* fonts;
    struct directory* dir;
    int tagsAdded;
    int dirPushed;
    int backPushed;
};

/*-------------------------------------------------------------------------------------------------
    main functions
-------------------------------------------------------------------------------------------------*/

void freeFonts(Font* fonts);
// struct directory* mainFrontend(Font*, struct directory*, int, int*);
struct dataToShow* mainFrontend(struct dataToShow* data, struct list* backList);
int windowShouldUpdate();
Font* initFrontend();

#endif