#ifndef __DATA_H
#define __DATA_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#include "structures/list.h"
#include "structures/dynarray.h"

struct directory {
    char* dirName;
    char* dirPath;
    struct list* directories;
    struct dynarray* songs;
};

struct song {
    char* songName;
    char* fileName;
    char* fileType;
    char* title;
    char* artist;
};

void freeDirectory(struct directory* dirName);
struct directory* getMusic(char* musicDirName, int updateMD);

#endif