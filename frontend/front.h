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
    int tagsAdded;
    int findDups;
    int backPushed;
    struct dynarray* duplicates;
    int showDuplicates;
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