#include "duplicates.h"

/*-------------------------------------------------------------------------------------------------
    free functions
-------------------------------------------------------------------------------------------------*/

void freeDup(struct duplicate* dup){
    dynarray_free(dup->locations);
    free(dup);
}

void freeDups(struct dynarray* dupArr){
    int i;
    for (i = 0; i < dynarray_size(dupArr); i++) {
        struct duplicate* dup = (struct duplicate*)dynarray_get(dupArr, i);
        freeDup(dup);
    }
    dynarray_free(dupArr);
}

/*-------------------------------------------------------------------------------------------------
    create function
-------------------------------------------------------------------------------------------------*/

struct duplicate* createDuplicate(char* title, unsigned long hash, char* location) {
    struct duplicate* dup = (struct duplicate*)calloc(1, sizeof(struct duplicate));
    dup->title = title;
    dup->hash = hash;
    dup->locations = dynarray_create();
    dynarray_insert(dup->locations, location);
    return dup;
}

/*-------------------------------------------------------------------------------------------------
    Hash functions
-------------------------------------------------------------------------------------------------*/

//djb2 hash function
unsigned long hashString(char* string) {
    unsigned long hash = 5381;
    int n;
    while ((n = *string++)){
        hash = ((hash << 5) + hash) + n;
    }
    return hash;
}

//checks if the hash is in the dynamic array
int isHashIn(unsigned long hash, struct dynarray* dupArr) {
    int i;
    for (i = 0; i < dynarray_size(dupArr); i++) {
        struct duplicate* dup = (struct duplicate*)dynarray_get(dupArr, i);
        if (hash == dup->hash) {
            return i;
        }
    }
    return -1;
}

/*-------------------------------------------------------------------------------------------------
    Find duplicates helpers
-------------------------------------------------------------------------------------------------*/

void sortSong(struct song* song, char* dirPath, struct dynarray* dupArr) {
    unsigned long hash;
    //hash the song
    if(song->title) {
        hash = hashString(song->title);
    } else {
        hash = hashString(song->songName);
    }
    int pos = isHashIn(hash, dupArr);
    struct duplicate* dup;
    if (pos != -1) {
        dup = dynarray_get(dupArr, pos);
        if (strcmp(song->title, dup->title) == 0) {
            //if the song is a duplicate
            dynarray_insert(dup->locations, dirPath);
            return;
        }
    }
    dup = createDuplicate(song->title, hash, dirPath);
    dynarray_insert(dupArr, dup);
}

void sortSongs(struct directory* directory, struct dynarray* dupArr) {
    int i;
    //recursive call to all subdirectories
    if(directory->directories) {
        for (i = 0; i < list_getNumElements(directory->directories); i++){
            struct directory* newDir = list_getElement(directory->directories, i);
            sortSongs(newDir, dupArr);
        }
    }
    //actually do the song stuff
    if(directory->songs) {
        for(i = 0; i < dynarray_size(directory->songs); i++){
            struct song* song = dynarray_get(directory->songs, i);
            sortSong(song, directory->dirPath, dupArr);
        }
    }
}

void consolidateDups(struct dynarray* dupArr) {
    int i;
    for (i = 0; i < dynarray_size(dupArr); i++) {
        struct duplicate* dup = (struct duplicate*)dynarray_get(dupArr, i);
        if (dynarray_size(dup->locations) == 1) {
            dynarray_remove(dupArr, i);
            freeDup(dup);
            i--;
        }
    }
}

/*-------------------------------------------------------------------------------------------------
    Main function
-------------------------------------------------------------------------------------------------*/

struct dynarray* findDuplicates(struct directory* directory) {
    //err handler
    if (!directory) {
        printf("no directory.\n");
        return NULL;
    }
    //get the array of hased songs
    struct dynarray* dupArr = dynarray_create();
    sortSongs(directory, dupArr);
    //remove non-duplicates
    consolidateDups(dupArr);
    return dupArr;
}