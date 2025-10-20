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
    struct directory* dir;
    int info[4];
    struct dynarray* duplicates;
};

/*-------------------------------------------------------------------------------------------------
    main functions
-------------------------------------------------------------------------------------------------*/

void freeFonts(Font* fonts);
struct dataToShow* mainFrontend(struct dataToShow* data, struct list* backList);
int windowShouldUpdate();
Font* initFrontend();

#endif