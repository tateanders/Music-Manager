#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

#include "metadata.h"

/*-------------------------------------------------------------------------------------------------
    Check format functions
-------------------------------------------------------------------------------------------------*/

int getID3v(FILE* file) {
    char id3v2_header[6]; // "ID3" + major + minor + flags
    fseek(file, 0, SEEK_SET);
    fread(id3v2_header, 1, 6, file);
    
    if (memcmp(id3v2_header, "ID3", 3) == 0) {
        
        if (id3v2_header[3] == 3) {
            return 23;
        } else if (id3v2_header[3] == 4) {
            return 24;
        }
    }
    return 0; // No ID3v2 tag found
}

/*-------------------------------------------------------------------------------------------------
    Get title and artist functions
-------------------------------------------------------------------------------------------------*/

char* v2dot3Title(const struct ID3v2dot3MetaData* data) {
    int i;
    int daSize = dynarray_size(data->frames);
    char* title = NULL;
    for (i = 0; i < daSize; i++) {
        struct ID3v2dot3Frame* frame = (struct ID3v2dot3Frame*)dynarray_get(data->frames, i);
        if (strncmp("TIT2", frame->id, 4) == 0) {
            title = calloc((size_t)frame->size, sizeof(char));
            memcpy(title, frame->data + 1, (size_t)frame->size - 1);
            break;
        }
    }
    return title;
}

char* v2dot3Artist(const struct ID3v2dot3MetaData* data) {
    int i;
    int daSize = dynarray_size(data->frames);
    char* artist = NULL;
    for (i = 0; i < daSize; i++) {
        struct ID3v2dot3Frame* frame = (struct ID3v2dot3Frame*)dynarray_get(data->frames, i);
        if (strncmp("TPE1", frame->id, 4) == 0) {
            artist = calloc((size_t)frame->size, sizeof(char));
            memcpy(artist, frame->data + 1, (size_t)frame->size - 1);
            break;
        }
    }
    return artist;
}

char* v2dot4Title(const struct ID3v2dot4MetaData* data) {
    int i;
    int daSize = dynarray_size(data->frames);
    char* title = NULL;
    for (i = 0; i < daSize; i++) {
        struct ID3v2dot4Frame* frame = (struct ID3v2dot4Frame*)dynarray_get(data->frames, i);
        if (strncmp("TIT2", frame->id, 4) == 0) {
            title = calloc((size_t)frame->size, sizeof(char));
            memcpy(title, frame->data + 1, (size_t)frame->size - 1);
            break;
        }
    }
    return title;
}

char* v2dot4Artist(const struct ID3v2dot4MetaData* data) {
    int i;
    int daSize = dynarray_size(data->frames);
    char* artist = NULL;
    for (i = 0; i < daSize; i++) {
        struct ID3v2dot4Frame* frame = (struct ID3v2dot4Frame*)dynarray_get(data->frames, i);
        if (strncmp("TPE1", frame->id, 4) == 0) {
            artist = calloc((size_t)frame->size, sizeof(char));
            memcpy(artist, frame->data + 1, (size_t)frame->size - 1);
            break;
        }
    }
    return artist;
}
/*-------------------------------------------------------------------------------------------------
    Main Functions
-------------------------------------------------------------------------------------------------*/

// int addComment(struct dirent* entry, char* comment){
//     //open the file with read and binary permissions
//     FILE* file = fopen(entry->d_name, "r+b");
//     if (!file) {
//         printf("Error opening \"%s\"\n", entry->d_name);
//         return 0;
//     }

//     //get the tag
//     int ID3v = getID3v(file);
//     if (!ID3v) {
//         return 0;
//     }

//     //add the comment
//     if (ID3v == 23) {
//         struct ID3v2dot3MetaData* data = getMetaDataV2dot3(file);
//         if (data) {
//             addCommentV2dot3(file, comment, data);
//             freeDataV2dot3(data);
//         }
//     } else if (ID3v == 24) {
//         struct ID3v2dot4MetaData* data = getMetaDataV2dot4(file);
//         if (data) {
//             addCommentV2dot4(file, comment, data);
//             freeDataV2dot4(data);
//         }
//     }

//     fclose(file);
//     return ID3v;
// }

int getSongData(struct dirent* entry, struct song* song, char* comment) {
    //open the file with read and binary permissions
    FILE* file = fopen(entry->d_name, "r+b");
    if (!file) {
        printf("Error opening \"%s\"\n", entry->d_name);
        return 0;
    }

    //get the tag
    int ID3v = getID3v(file);
    if (!ID3v) {
        return 0;
    }

    //add the comment
    if (ID3v == 23) {
        struct ID3v2dot3MetaData* data = getMetaDataV2dot3(file);
        if (data) {
            if (comment) {
                addCommentV2dot3(file, comment, data);
            }
            song->title = v2dot3Title(data);
            song->artist = v2dot3Artist(data);
            freeDataV2dot3(data);
        }
    } else if (ID3v == 24) {
        struct ID3v2dot4MetaData* data = getMetaDataV2dot4(file);
        if (data) {
            if (comment) {
                addCommentV2dot4(file, comment, data);
            }
            song->title = v2dot4Title(data);
            song->artist = v2dot4Artist(data);
            freeDataV2dot4(data);
        }
    }

    fclose(file);
    return ID3v;
}