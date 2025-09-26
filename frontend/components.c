#include "components.h"
#include <inttypes.h>

/*-------------------------------------------------------------------------------------------------
    helper functions
-------------------------------------------------------------------------------------------------*/

Clay_String buildClayString(const char* string) { /*static inline*/
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
    Clay_ElementId backButtonId = Clay__HashString(CLAY_STRING("Back Button Container"), 0, 0);
    CLAY({
        .id = backButtonId,
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
    CLAY({
        .id = CLAY_ID("Header"),
        .layout = {
            .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(100) },  // full width, fixed height
            .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER },
            //.padding = CLAY_PADDING_ALL(12)
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            // .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
            // .childGap = 10
        },
        .backgroundColor = GARNET,  // dark grey
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
    render song and directory buttons
-------------------------------------------------------------------------------------------------*/

void renderSongButton(struct song* song, int pos) {
    // Create unique ID for each button
    Clay_String text = buildClayString(song->songName);
    Clay_ElementId dirId = Clay__HashString(text, pos, 0);
    
    CLAY({
        .id = dirId,
        .layout = { 
            .sizing = { 
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_FIXED(60) // Explicit height
            },
            .padding = CLAY_PADDING_ALL(16),
            .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER } 
        },
        .backgroundColor = OLDGOLD,
        .cornerRadius = 8,
    }) {
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({ 
            .fontId = GOTHIC, 
            .fontSize = 40, 
            .textColor = GARNET
        }));
    }
}

void renderDirButton(struct directory* dir, int pos) {
    // Create unique ID for each button
    Clay_String text = buildClayString(dir->dirName);
    Clay_ElementId dirId = Clay__HashString(text, pos, 0);
    
    CLAY({
        .id = dirId,
        .layout = { 
            .sizing = { 
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_FIXED(60) // Explicit height
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
    // Create unique ID for each button
    Clay_String text = buildClayString(dir->dirPath);
    Clay_ElementId dirId = Clay__HashString(text, pos, 0);
    
    CLAY({
        .id = dirId,
        .layout = { 
            .sizing = { 
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_FIXED(60) // Explicit height
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
    CLAY({
        .layout = { 
            .sizing = { 
                .width = CLAY_SIZING_GROW(0),
                .height = CLAY_SIZING_FIXED(60) // Explicit height
            },
            .padding = CLAY_PADDING_ALL(16),
            .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER } 
        },
        .backgroundColor = GREENS,
        .cornerRadius = 8,
    }) {
        CLAY_TEXT(CLAY_STRING("Adding MetaData"), CLAY_TEXT_CONFIG({ 
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
    render the sideBar button
-------------------------------------------------------------------------------------------------*/

void renderGreenButton() {
    Clay_ElementId bId = Clay__HashString(CLAY_STRING("Tags Added"), 0, 0);
    CLAY({
        .id = bId,
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

void renderTagsButton() {
    Clay_ElementId bId = Clay__HashString(CLAY_STRING("Add Tags"), 0, 0);
    CLAY({
        .id = bId,
        .layout = {
            .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER },
            .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(75) },
        },
        .backgroundColor = BLUEGRAY,
        .cornerRadius = 8,
    }){
        CLAY_TEXT(CLAY_STRING("Add Tags"), CLAY_TEXT_CONFIG({
            .fontId = GOTHIC,
            .fontSize = 32,
            .textColor = OLDGOLD
        }));
    }
}

void renderSidebarButton(int wasPressed) {
    if (wasPressed) {
        renderGreenButton();
    } else {
        renderTagsButton();
    }
}
