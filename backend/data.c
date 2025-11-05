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
        // free(song->fileType);
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

/*-------------------------------------------------------------------------------------------------
    Helper functions
-------------------------------------------------------------------------------------------------*/

char* getWorkingDir() {
    static char cwd[PATH_MAX];  // static so it persists after the function returns
    getcwd(cwd, sizeof(cwd));

    char *lastSlash = strrchr(cwd, '/');
    if (lastSlash && *(lastSlash + 1) != '\0') {
        return lastSlash + 1;  // return everything after the last '/'
    } else {
        return cwd;  // fallback if no slash found
    }
}

DIR* openMusicDir(char* music){
    char cwp[PATH_MAX];  // static so it persists after the function returns
    getcwd(cwp, sizeof(cwp));

    //if we have to go back to the desktop
    if (strstr(cwp, "Desktop") != NULL) {
        char* cwd = getWorkingDir();
        while(strcmp(cwd, "Desktop") != 0) {
            chdir("..");
            cwd = getWorkingDir();
        }
    } else {
        //if we have to go forward
        chdir("Desktop");
    }

    //open the directory
    DIR* musicDir = opendir(music);
    if (!musicDir) {
        perror("no \"mp3\" directory found on desktop\n");
        exit(1);
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

//if the song has a .mp3 extension, returns where the extension starts, otherwise returns 0
int hasMP3(char* string) {
    char suffix[5] = ".mp3\0";
    size_t stringLen = strlen(string);
    size_t suffLen = strlen(suffix);
    //position where the .mp3 would begin
    int pos = (int)(stringLen - suffLen);

    if (strncmp(string + pos, suffix, suffLen) == 0) {
        return pos;
    }
    return 0;
}

struct song* createSong(struct dirent* entry, char* dirPath, int updateMD){
    struct song* song = (struct song*) calloc(1, sizeof(struct song));

    //set the file name
    song->fileName = strdup(entry->d_name);

    //set the song name
    int MP3Pos = hasMP3(song->fileName);
    if (MP3Pos) {
        song->songName = strndup(song->fileName, (size_t)MP3Pos);
    } else {
        song->songName = strdup(song->fileName);
    }

    //replace the '_' with ' '
    replaceChars(song->songName);

    //add comments and get title/artist metadata
    char* comment = NULL;
    if (updateMD == 1) {
        comment = dirPath;
    }
    getSongData(entry, song, comment);
    if (!song->title){
        song->title = strdup(song->songName);
    }
    if (!song->artist) {
        song->artist = getSmallString();
    }

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
        printf("TITLE: %s |\n", song->songName);
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
    Main function
-------------------------------------------------------------------------------------------------*/

struct directory* getMusic(char* musicDirName, int updateMD) {
    //enter music directory
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

    // printNums();

    return music;
}