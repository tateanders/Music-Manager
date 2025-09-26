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

/*-------------------------------------------------------------------------------------------------
    main functions
-------------------------------------------------------------------------------------------------*/

void freeFonts(Font* fonts);
struct directory* mainFrontend(Font*, struct directory*, int, int*);
int windowShouldUpdate();
Font* initFrontend();

#endif