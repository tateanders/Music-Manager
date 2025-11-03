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
    return 0;
}

/*-------------------------------------------------------------------------------------------------
    Conversions to utf8
-------------------------------------------------------------------------------------------------*/

int checkISO(const uint8_t* data) {
    if(data[0] == 0) {
        return 1;
    }
    return 0;
}

int checkUTF16(const uint8_t* data) {
    if(data[0] == 1) {
        return 1;
    }
    return 0;
}

void ISO_UTF8(const uint8_t* data, const size_t dataSize, char* title) {
    if (!data || !title) return;

    char* out = title;
    for (size_t i = 0; i < dataSize; i++) {
        unsigned char c = data[i];
        if (c < 0x80) {
            *out++ = c;
        } else {
            *out++ = 0xC0 | (c >> 6);
            *out++ = 0x80 | (c & 0x3F);
        }
    }
    *out = '\0';
}

void UTF16_UTF8(const uint8_t* data, size_t dataSize, char* title) {
    if (!data || !title || dataSize < 2) return;

    // Detect BOM
    int littleEndian = 1;
    if (data[0] == 0xFE && data[1] == 0xFF) {
        littleEndian = 0;  // Big-endian
        data += 2;
        dataSize -= 2;
    } else if (data[0] == 0xFF && data[1] == 0xFE) {
        littleEndian = 1;  // Little-endian
        data += 2;
        dataSize -= 2;
    }

    size_t utf16Len = dataSize / 2;
    const uint16_t* utf16 = (const uint16_t*)data;

    char* out = title;

    for (size_t i = 0; i < utf16Len; i++) {
        uint16_t code = littleEndian ? utf16[i] : (utf16[i] >> 8) | (utf16[i] << 8);

        if (code <= 0x7F) {
            *out++ = (char)code;
        } else if (code <= 0x7FF) {
            *out++ = 0xC0 | ((code >> 6) & 0x1F);
            *out++ = 0x80 | (code & 0x3F);
        } else {
            *out++ = 0xE0 | ((code >> 12) & 0x0F);
            *out++ = 0x80 | ((code >> 6) & 0x3F);
            *out++ = 0x80 | (code & 0x3F);
        }
    }
    *out = '\0';
}

/*-------------------------------------------------------------------------------------------------
    Get title and artist functions
-------------------------------------------------------------------------------------------------*/

void constructUTF8(const uint8_t* data, size_t dataSize, char* title) {
    if (checkISO(data)) {
        //if the encoding is ISO-8859-1:
        ISO_UTF8(data + 1, dataSize - 1, title);
    } else if (checkUTF16(data)){
        //if the encoding is UTF16
        UTF16_UTF8(data + 1, dataSize - 1, title);
    } else {
        memcpy(title, data + 1, (size_t)dataSize - 1);
    }
}

char* v2dot3Title(const struct ID3v2dot3MetaData* data) {
    int i;
    int daSize = dynarray_size(data->frames);
    char* title = NULL;
    for (i = 0; i < daSize; i++) {
        struct ID3v2dot3Frame* frame = (struct ID3v2dot3Frame*)dynarray_get(data->frames, i);
        if (strncmp("TIT2", frame->id, 4) == 0) {
            title = calloc((size_t)(3 * (frame->size / 2)), sizeof(char));
            constructUTF8(frame->data, frame->size, title);
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
            artist = calloc((size_t)(3 * (frame->size / 2)), sizeof(char));
            constructUTF8(frame->data, frame->size, artist);
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
            title = calloc((size_t)(3 * (frame->size / 2)), sizeof(char));
            constructUTF8(frame->data, frame->size, title);
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
            artist = calloc((size_t)(3 * (frame->size / 2)), sizeof(char));
            constructUTF8(frame->data, frame->size, artist);
            break;
        }
    }
    return artist;
}
/*-------------------------------------------------------------------------------------------------
    Main Function
-------------------------------------------------------------------------------------------------*/

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