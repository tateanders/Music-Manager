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

int mainFrontend(Font*, struct directory*);
int windowShouldUpdate();
Font* initFrontend();

#endif