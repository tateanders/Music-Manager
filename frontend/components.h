#ifndef __COMPONENTS_H
#define __COMPONENTS_H
#include "clay.h"
#include "data.h"
#include "front.h"
#include "globals.h"

Clay_String buildClayString(const char* string);
void renderHeader(int);
void renderSongButton(struct claySong* song, int pos);
void renderDirButton(struct clayDirectory* dir, int pos);
void renderDirectory(struct clayDirectory* dir);
void renderSidebarButtons(int tags, int dups);
void renderDirHeader2();
void renderDups(struct dynarray* duplicates);

#endif