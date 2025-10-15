#include "data.h"
#include "metadata.h"
#include <limits.h>

/*-------------------------------------------------------------------------------------------------
    Free functions
-------------------------------------------------------------------------------------------------*/

char* getSmallString(){
    char* string = calloc(3, sizeof(char));
    string[0] = '-';
    string[1] = '-';
    return string;
}

void replaceChars(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '_') {
            str[i] = ' ';
        }
    }
}

void freeSongs(struct dynarray* songs){
    struct song* song;
    int i;
    for (i = 0; i < dynarray_size(songs); i++){
        song = (struct song*)dynarray_get(songs, i);
        free(song->fileName);
        free(song->songName);
        free(song->fileType);
        free(song->title);
        free(song->artist);
        free(song);
    }
    dynarray_free(songs);
    return;
}

void freeDirectory(struct directory* directory){
    if(directory->songs){
        freeSongs(directory->songs);
    }
    if(directory->directories){
        struct directory* dir2 = (struct directory*)list_pop(directory->directories);
        while(dir2){
            freeDirectory(dir2);
            dir2 = (struct directory*)list_pop(directory->directories);
        }
        list_free(directory->directories);
    }
    free(directory->dirName);
    free(directory->dirPath);
    free(directory);
    return;
}

void freeDups(struct dynarray* dups) {
    int i;
    int n;
    for (i = 0; i < n; i++) {
        struct duplicate* dup = (struct duplicate*)dynarray_get(dups, i);
        dynarray_remove(dups, i);
        free(dup->hash);
        free(dup);
    }
    dynarray_free(dups);
}

// char* truncateString(const char* input) {
//     size_t len = strlen(input);
//     if (len <= 38) {
//         char* copy = malloc(len + 1);
//         if (!copy) return NULL;
//         memcpy(copy, input, len + 1); // include null terminator
//         return copy;
//     } else {
//         // 17 chars + "..." (3 chars) + '\0' = 21 bytes
//         char* truncated = malloc(35 + 3 + 1);
//         if (!truncated) return NULL;
//         memcpy(truncated, input, 35);       // copy first 17 bytes
//         memcpy(truncated + 35, "...", 4);   // copy '.' '.' '.' '\0'
//         return truncated;
//     }
// }

/*-------------------------------------------------------------------------------------------------
    Helper functions
-------------------------------------------------------------------------------------------------*/

DIR* openMusicDir(char* music){
    //make sure we are in desktop
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    //gave up dont care will fix *eventually*

    DIR* musicDir = opendir(music);
    if (!musicDir) {
        chdir("Desktop");
        musicDir = opendir(music);
        if (!musicDir) {
            perror("no \"music\" directory found on desktop\n");
            exit(1);
        }
    }
    chdir(music);
    return musicDir; 
}

int isDirectory(struct dirent* entry){
    if (entry->d_type == DT_DIR) {
        return 1;
    } else if (entry->d_type == DT_REG) {
        return 0;
    }
    printf("Invalid format for '%s' (retruning -1)\n", entry->d_name);
    return -1;
}

/*-------------------------------------------------------------------------------------------------
    Song functions
-------------------------------------------------------------------------------------------------*/

struct song* createSong(struct dirent* entry, char* dirPath, int updateMD){
    struct song* song = (struct song*) calloc(1, sizeof(struct song));
    //set the file name
    song->fileName = (char*) calloc(strlen(entry->d_name) + 1, sizeof(char));
    strcpy(song->fileName, entry->d_name);
    //get the songs name and null terminate it
    size_t len;
    const char* dot = strrchr(song->fileName, '.'); // find last dot
    //stuff
    if (dot && dot != song->fileName) {
        //dot found
        len = (size_t) (dot - song->fileName);
        song->songName = (char*)calloc(len + 1, sizeof(char));
        strncpy(song->songName, song->fileName, len);
        song->songName[len] = '\0';
    } else {
        //no extension found
        song->songName = strdup(song->fileName);
        song->fileType = strdup("");
    }

    //get the songs file extension
    len = (strlen(song->fileName) - strlen(song->songName));
    song->fileType = (char*) calloc(len, sizeof(char));
    strcpy(song->fileType, dot + 1);
    //truncate the name of the song
    // char* newName = truncateString(song->songName);
    // free(song->songName);
    // song->songName = newName;
    replaceChars(song->songName);
    //add comments
    char* comment = NULL;
    if (updateMD == 1) {
        comment = dirPath;
    }
    getSongData(entry, song, comment);
    if (song->title) {
        replaceChars(song->title);
    } else {
        song->title = getSmallString();
    }
    if (!song->artist) {
        song->artist = getSmallString();
    }

    //debug prints
    // printf("\nSize: %lu, SongName: %s\n", strlen(song->songName), song->songName);
    // printf("Size: %lu, Title: %s\n", strlen(song->title), song->title);
    // printf("Size: %lu, Artist: %s\n", strlen(song->artist), song->artist);

    return song;
}

/*-------------------------------------------------------------------------------------------------
    Directory functions
-------------------------------------------------------------------------------------------------*/

void addPath(char* dirName, char** dirPath){
    size_t newLen;
    if (*dirPath){
        newLen = strlen(*dirPath) + strlen(dirName) + 2;
    } else {
        newLen = strlen(dirName) + 2;
    }
    char* newPath = (char*)calloc(newLen, sizeof(char));
    if (*dirPath){
        strcpy(newPath, *dirPath);
        strcat(newPath, dirName);
        strcat(newPath, "/");
        free(*dirPath);
    } else {
        strcpy(newPath, dirName);
        strcat(newPath, "/");
    }
    *dirPath = newPath;
}

struct directory* createDirectory(char* dirName, char* dirPath){
    //create the directory
    struct directory* directory = (struct directory*) calloc(1, sizeof(struct directory));
    //add the name to the directory
    directory->dirName = (char*) calloc(strlen(dirName) + 1, sizeof(char));
    strcpy(directory->dirName, dirName);
    //add the path to the directory
    if (dirPath) {
        directory->dirPath = (char*) calloc(strlen(dirPath) + 1, sizeof(char));
        strcpy(directory->dirPath, dirPath);
    } else {
        directory->dirPath = NULL;
    }
    addPath(dirName, &directory->dirPath);
    directory->directories = NULL;
    directory->songs = NULL;
    return directory;
}

struct directory* fillDirectory(DIR* dir, char* dirName, char* dirPath, int updateMD){
    struct directory* directory = createDirectory(dirName, dirPath);
    struct dirent* entry;
    do {
        //read the entry
        entry = readdir(dir);
        //if the entry exists and is not a hidden file
        if (entry && (entry->d_name[0] != '.')){
            if(isDirectory(entry)){ //if this entry is a directory
                //if a directories list doesnt already exist
                if (!directory->directories){
                    directory->directories = (struct list*) list_create();
                }
                //create and insert a new directory
                DIR* newDirectory = opendir(entry->d_name);
                chdir(entry->d_name);
                struct directory* newDir = fillDirectory(newDirectory, (char*)entry->d_name, directory->dirPath, updateMD);
                list_insert(directory->directories, newDir);
                chdir("..");
                closedir(newDirectory);

            } else if (isDirectory(entry) == 0){ //if this entry is a song
                //if the songs array doesnt exist
                if (!directory->songs){
                    directory->songs = (struct dynarray*) dynarray_create();
                }
                //create and insert a new song
                // printf("\n\nDirectory: %s\n", directory->dirPath);
                struct song* song = createSong(entry, directory->dirPath, updateMD);
                dynarray_insert(directory->songs, song);
            }
        }
    } while (entry);
    return directory;
}

/*-------------------------------------------------------------------------------------------------
    Print functions
-------------------------------------------------------------------------------------------------*/

void printSpaces(int numSpaces){
    int i;
    for (i = 0; i < numSpaces; i++){
        printf(" ");
    }
}

void printSongs(struct dynarray* songs, int numSpaces){
    printSpaces(numSpaces);
    printf("SONGS:\n");
    struct song* song;
    int i;
    for (i = 0; i < dynarray_size(songs); i++){
        printSpaces(numSpaces);
        song = (struct song*) dynarray_get(songs, i);
        printf("TITLE: %s | TYPE: %s\n", song->songName, song->fileType);
    }
}

void printDirectory(struct directory* directory, int numSpaces){
    printSpaces(numSpaces);
    printf("DIRECTORY: %s, PATH: %s\n", directory->dirName, directory->dirPath);
    numSpaces++;
    if(directory->directories) {
        int i;
        for (i = 0; i < list_getNumElements(directory->directories); i++){
            struct directory* dir2 = (struct directory*) list_getElement(directory->directories, i);
            printDirectory(dir2, numSpaces);
        }
    } else if (directory->songs) {
        printSongs(directory->songs, numSpaces);
    }
    numSpaces--;
}

/*-------------------------------------------------------------------------------------------------
    Find duplicates helper
-------------------------------------------------------------------------------------------------*/

//djb2 hash function
unsigned long hashString(char* string) {
    unsigned long hash = 5381;
    int n;
    while ((n = *string++))
        hash = ((hash << 5) + hash) + n;
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

void hashSong (struct song* song, char* dirPath, struct dynarray* hashArr) {
    unsigned long songHash;
    struct duplicate* dup;
    //hash the song
    if(song->title) {
        songHash = hashString(song->title);
    } else {
        songHash = hashString(song->songName);
    }
    int pos = isHashIn(songHash, hashArr);
    //if the song is not a duplicate
    if (pos == -1) {
        dup = calloc(1, sizeof(struct duplicate));
        if(song->title) {
            dup->title = song->title;
        } else {
            dup->title = song->songName;
        }
        dup->hash = calloc(1, sizeof(unsigned long));
        dup->hash = &songHash;
        dup->locations = dynarray_create();
        dynarray_insert(dup->locations, dirPath);
        dup->num = 1;

        dynarray_insert(hashArr, dup);
    } else {
        dup = dynarray_get(hashArr, pos);
        dynarray_insert(dup->locations, dirPath);
        dup->num++;
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
            free(dup->hash);
            free(dup);
        }
    }
}

/*-------------------------------------------------------------------------------------------------
    Main function
-------------------------------------------------------------------------------------------------*/

struct directory* getMusic(char* musicDirName, int updateMD) {
    //enter music directory
    chdir("..");
    chdir("..");
    DIR* musicDir = openMusicDir(musicDirName); 

    //get contents of music directory
    if (updateMD) {
        printf("Updating Metadata\n");
    }
    struct directory* music = fillDirectory(musicDir, musicDirName, NULL, updateMD);
    if (updateMD == 1) {
        printf("Metadata Updated\n");
    }
    closedir(musicDir);

    return music;
}

struct dynarray* findDuplicates(struct directory* directory) {
    //err handler
    if (!directory) {
        printf("Directory not given.\n");
        return NULL;
    }
    //get the array of hased songs
    struct dynarray* hashArr = dynarray_create();
    hashSongs(directory, hashArr);
    //remove non-duplicates
    consolidateDups(hashArr);
    return hashArr;
}