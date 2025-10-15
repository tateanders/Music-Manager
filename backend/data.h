#ifndef __DATA_H
#define __DATA_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

#include "structures/list.h"
#include "structures/dynarray.h"
// #include "metadata.h"

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

struct duplicate {
    char* title;
    unsigned long* hash;
    struct dynarray* locations;
    int num;
};

void freeDirectory(struct directory* dirName);
void freeDups(struct dynarray* dups);
struct directory* getMusic(char* musicDirName, int updateMD);
struct dynarray* findDuplicates(struct directory* directory);

#endif