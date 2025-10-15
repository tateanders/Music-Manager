#include "duplicates.h"

/*-------------------------------------------------------------------------------------------------
    free functions
-------------------------------------------------------------------------------------------------*/

void freeDup(struct duplicate* dup){
    dynarray_free(dup->locations);
    free(dup);
}

void freeDups(struct dynarray* dups){
    int i;
    int n;
    for (i = 0; i < n; i++) {
        struct duplicate* dup = (struct duplicate*)dynarray_get(dups, i);
        dynarray_remove(dups, i);
        printf("debug1\n");
        free(dup);
    }
    printf("debug2\n");
    dynarray_free(dups);
    printf("debug3\n");
}

/*-------------------------------------------------------------------------------------------------
    Find duplicates helpers
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
int isHashIn(unsigned long hash, struct dynarray* arr) {
    int arrSize = dynarray_size(arr);
    int i;
    for (i = 0; i < arrSize; i++) {
        unsigned long* arrHash = (unsigned long*)dynarray_get(arr, i);
        if (hash == *arrHash) {
            return i;
        }
    }
    return -1;
}

void hashSong(struct song* song, char* dirPath, struct dynarray* hashArr) {
    unsigned long songHash;
    struct duplicate* dup = calloc(1, sizeof(struct duplicate));
    //hash the song
    if(song->title) {
        dup->hash = hashString(song->title);
    } else {
        dup->hash = hashString(song->songName);
    }
    int pos = isHashIn(dup->hash, hashArr);
    //if the song is not a duplicate
    if (pos == -1) {
        if(song->title) {
            dup->title = song->title;
        } else {
            dup->title = song->songName;
        }
        dup->locations = dynarray_create();
        dynarray_insert(dup->locations, dirPath);
        dup->num = 1;

        dynarray_insert(hashArr, dup);
    } else {
        free(dup);
        struct duplicate* dup2 = dynarray_get(hashArr, pos);
        dynarray_insert(dup2->locations, dirPath);
        dup2->num++;
    }
}

void hashSongs(struct directory* directory, struct dynarray* hashArr) {
    int i;
    //recursive call to all subdirectories
    if(directory->directories) {
        int numDirs = list_getNumElements(directory->directories);
        for (i = 0; i < numDirs; i++){
            struct directory* newDir = list_getElement(directory->directories, i);
            hashSongs(newDir, hashArr);
        }
    }
    //actually do the song stuff
    if(directory->songs) {
        int numSongs = dynarray_size(directory->songs);
        for(i = 0; i < numSongs; i++){
            struct song* song = dynarray_get(directory->songs, i);
            hashSong(song, directory->dirPath, hashArr);
        }
    }
}

void consolidateDups(struct dynarray* hashArr) {
    int i;
    int n = dynarray_size(hashArr);
    for (i = 0; i < n; i++) {
        struct duplicate* dup = (struct duplicate*)dynarray_get(hashArr, i);

        if (dup->num > 1) {
            dynarray_remove(hashArr, i);
            // free(dup->hash);
            free(dup);
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
    struct dynarray* hashArr = dynarray_create();
    hashSongs(directory, hashArr);
    //remove non-duplicates
    consolidateDups(hashArr);
    return hashArr;
}