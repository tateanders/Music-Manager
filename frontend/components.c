#include "components.h"
#include <inttypes.h>

/*-------------------------------------------------------------------------------------------------
    helper functions
-------------------------------------------------------------------------------------------------*/

Clay_String buildClayString(const char* string) {
    Clay_String clayString;
    clayString.isStaticallyAllocated = false;
    clayString.length = (int32_t)strlen(string);
    clayString.chars = string;
    return clayString;
}

/*-------------------------------------------------------------------------------------------------
    render header bar
-------------------------------------------------------------------------------------------------*/

void renderBackButton() {
    Clay_ElementId backButtonId = Clay__HashString(CLAY_STRING("Back Button Container"), 0);
    CLAY( backButtonId, {
        .layout = {
            .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER },
            .sizing = { CLAY_SIZING_FIXED(100), CLAY_SIZING_GROW(0) },
            .padding = { .left = 10 }
        },
        .floating = { .attachTo = CLAY_ATTACH_TO_PARENT },
        .backgroundColor = GARNET,
    }){
        CLAY_TEXT(CLAY_STRING("Back"), CLAY_TEXT_CONFIG({
            .fontId = GOTHIC,
            .fontSize = 40,
            .textColor = OLDGOLD
        }));
    }
}

void renderHeader(int goBack) {
    // HEADER
    CLAY(CLAY_ID("Header"), {
        .layout = {
            .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(100) },  // full width, fixed height
            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER, .x = CLAY_ALIGN_X_CENTER},
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
        },
        .backgroundColor = GARNET
    }){
        if (goBack) {
            renderBackButton();
        }
        CLAY_TEXT(CLAY_STRING("Music Library"), CLAY_TEXT_CONFIG({
            .fontId = GOTHIC,
            .fontSize = 65,
            .textColor = BLUEGRAY
        }));
    } 
}

/*-------------------------------------------------------------------------------------------------
    render duplicates
-------------------------------------------------------------------------------------------------*/

void renderDup(struct duplicate* dup, int pos) {
    char dest[128] = "Title: ";
    strcat(dest, dup->title);
    Clay_String title = buildClayString(dup->title);
    Clay_ElementId songId = Clay__HashString(title, pos);

    CLAY(songId, {
        .layout = { 
            .sizing = { 
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_GROW(0)
            },
            .padding = CLAY_PADDING_ALL(16),
            .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER },
            .layoutDirection = CLAY_TOP_TO_BOTTOM
        },
        .backgroundColor = OLDGOLD,
        .cornerRadius = 8,
    }) {
        CLAY_TEXT(title, CLAY_TEXT_CONFIG({ 
            .wrapMode = CLAY_TEXT_WRAP_WORDS,
            .fontId = GOTHIC, 
            .fontSize = 40, 
            .textColor = GARNET
        }));
        CLAY_TEXT(CLAY_STRING("Appears in:"), CLAY_TEXT_CONFIG({ 
            .fontId = GOTHIC, 
            .fontSize = 40, 
            .textColor = BLUEGRAY
        }));
        int i;
        for (i = 0; i < dynarray_size(dup->locations); i++) {
            Clay_String dir = buildClayString((char*)dynarray_get(dup->locations, i));
            CLAY_TEXT(dir, CLAY_TEXT_CONFIG({ 
                .fontId = GOTHIC, 
                .fontSize = 30, 
                .textColor = GARNET
            }));
        }
    }
}

void renderDupHeader(Clay_String text) {
    Clay_ElementId dirId = Clay__HashString(text, 0);
    
    CLAY( dirId, {
        .layout = { 
            .sizing = { 
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_FIXED(60)
            },
            .padding = CLAY_PADDING_ALL(16),
            .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER } 
        },
        .backgroundColor = BLUEGRAY,
        .cornerRadius = 8,
    }) {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({ 
            .fontId = GOTHIC, 
            .fontSize = 40, 
            .textColor = OLDGOLD
        }));
    }
}

void renderDups(struct dynarray* duplicates) {
    Clay_String text;
    int numDups = dynarray_size(duplicates);
    int i;
    if(!numDups) {
        //tell the user there are no duplicates
        text = buildClayString("No Duplicates Found.");
        renderDupHeader(text);
        return;
    }
    //show the header
    text = buildClayString("Duplicates found!");
    renderDupHeader(text);
    for (i = 0; i < numDups; i++) {
        struct duplicate* dup = dynarray_get(duplicates, i);
        renderDup(dup, i);
    }
}

/*-------------------------------------------------------------------------------------------------
    render song and directory buttons
-------------------------------------------------------------------------------------------------*/

void renderSongButton(struct song* song, int pos) {
    Clay_String songName = buildClayString(song->songName);
    Clay_ElementId songId = Clay__HashString(songName, pos);
    //get the artist and song
    Clay_String title = buildClayString(song->title);
    Clay_String artist = buildClayString(song->artist);
    //print shit
    CLAY(songId, {
        .layout = { 
            .sizing = { 
                .width = CLAY_SIZING_PERCENT(1), .height = CLAY_SIZING_GROW(0)
            },
            .padding = CLAY_PADDING_ALL(16),
            .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER },
            .layoutDirection = CLAY_TOP_TO_BOTTOM
        },
        .backgroundColor = OLDGOLD,
        .cornerRadius = 8,
    }) {
        CLAY_TEXT(title, CLAY_TEXT_CONFIG({ 
            .wrapMode = CLAY_TEXT_WRAP_WORDS,
            .fontId = NOTO, 
            .fontSize = 40, 
            .textColor = GARNET
        }));
        CLAY_TEXT(artist, CLAY_TEXT_CONFIG({ 
            .fontId = NOTOI, 
            .fontSize = 30, 
            .textColor = GARNET
        }));
    }
}

void renderDirButton(struct directory* dir, int pos) {
    // Create unique ID for each button
    Clay_String text = buildClayString(dir->dirName);
    Clay_ElementId dirId = Clay__HashString(text, pos);
    
    CLAY( dirId, {
        .layout = { 
            .sizing = { 
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_FIXED(60)
            },
            .padding = CLAY_PADDING_ALL(16),
            .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER } 
        },
        .backgroundColor = GARNET,
        .cornerRadius = 8,
    }) {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({ 
            .fontId = GOTHIC, 
            .fontSize = 40, 
            .textColor = OLDGOLD
        }));
    }
}

/*-------------------------------------------------------------------------------------------------
    render directory header
-------------------------------------------------------------------------------------------------*/

void renderDirHeader(struct directory* dir, int pos) {
    Clay_String text = buildClayString(dir->dirPath);
    Clay_ElementId dirId = Clay__HashString(text, pos);
    
    CLAY( dirId, {
        .layout = { 
            .sizing = { 
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_FIXED(60)
            },
            .padding = CLAY_PADDING_ALL(16),
            .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER } 
        },
        .backgroundColor = BLUEGRAY,
        .cornerRadius = 8,
    }) {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({ 
            .fontId = GOTHIC, 
            .fontSize = 40, 
            .textColor = OLDGOLD
        }));
    }
}

void renderDirHeader2() {
    CLAY_AUTO_ID({
        .layout = { 
            .sizing = { 
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_FIXED(60)
            },
            .padding = CLAY_PADDING_ALL(16),
            .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER } 
        },
        .backgroundColor = GREENS,
        .cornerRadius = 8,
    }) {
        CLAY_TEXT(CLAY_STRING("Working..."), CLAY_TEXT_CONFIG({ 
            .fontId = GOTHIC, 
            .fontSize = 40, 
            .textColor = GARNET
        }));
    }
}

/*-------------------------------------------------------------------------------------------------
    render the full directory
-------------------------------------------------------------------------------------------------*/

void renderDirectory(struct directory* dir) {
    int i;
    int pos = 0;
    renderDirHeader(dir, pos);
    //render directories
    if (dir->directories) {
        struct list* directories = dir->directories;
        for (i = 0; i < list_getNumElements(directories); i++) {
            struct directory* dirButton = (struct directory*)list_getElement(directories, i);
            renderDirButton(dirButton, pos);
            pos++;
        }
    }
    //render songs
    if (dir->songs) {
        struct dynarray* songs = dir->songs;
        for (i = 0; i < dynarray_size(songs); i++){
            struct song* song = dynarray_get(songs, i);
            renderSongButton(song,pos);
            pos++;
        }
    }
}

/*-------------------------------------------------------------------------------------------------
    render the sideBar buttons
-------------------------------------------------------------------------------------------------*/

void renderGreenButton() {
    Clay_ElementId bId = Clay__HashString(CLAY_STRING("x"), 0);
    CLAY( bId, {
        .layout = {
            .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER },
            .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(75) },
        },
        .backgroundColor = GREENS,
        .cornerRadius = 8,
    }){
        CLAY_TEXT(CLAY_STRING("Working"), CLAY_TEXT_CONFIG({
            .fontId = GOTHIC,
            .fontSize = 32,
            .textColor = GARNET
        }));
    }
}

void renderSidebarButton(Clay_String text) {
    Clay_ElementId bId = Clay__HashString(text, 0);
    bId = Clay__HashString(text, 0);
    CLAY( bId, {
        .layout = {
            .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER },
            .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(75) },
        },
        .backgroundColor = BLUEGRAY,
        .cornerRadius = 8,
    }){
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({
            .fontId = GOTHIC,
            .fontSize = 32,
            .textColor = OLDGOLD
        }));
    }
}

void renderSidebarButtons(int tags, int dups) {
    Clay_String tagString = buildClayString("Add Tags");
    Clay_String dupString = buildClayString("Find Dups");
    if (tags) {
        renderGreenButton();
        renderSidebarButton(dupString);
    } else if (dups) {
        renderSidebarButton(tagString);
        renderGreenButton();
    } else {
        renderSidebarButton(tagString);
        renderSidebarButton(dupString);
    }
}
