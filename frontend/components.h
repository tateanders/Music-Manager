#ifndef __COMPONENTS_H
#define __COMPONENTS_H
#include "clay.h"
#include "data.h"
#include "front.h"
#include "globals.h"

Clay_String buildClayString(const char* string);
void renderHeader(int);
void renderSongButton(struct song* song, int pos);
void renderDirButton(struct directory* dir, int pos);
void renderDirectory(struct directory* dir);
void renderSidebarButton(int wasPressed);

#endif