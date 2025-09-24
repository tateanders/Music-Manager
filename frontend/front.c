#define CLAY_IMPLEMENTATION
#include "front.h"
#include "clay_renderer_raylib.c"

/*-------------------------------------------------------------------------------------------------
    Fonts and Colors
-------------------------------------------------------------------------------------------------*/

//define the font names
const int GOTHIC = 0;
const int JAKARTA = 1;
const int PLAYFAIR = 2;
const int OSWALD = 3;

//colors
const Clay_Color DARKMODE = (Clay_Color) {36, 36, 36, 255};
const Clay_Color GARNET = (Clay_Color) {121, 36, 47, 255};
const Clay_Color BLUEGRAY = (Clay_Color) {43, 41, 51, 255};
const Clay_Color OLDGOLD = (Clay_Color) {170, 151, 103, 255};

/*-------------------------------------------------------------------------------------------------
    Error handler
-------------------------------------------------------------------------------------------------*/

void HandleClayErrors(Clay_ErrorData errorData) {
    // See the Clay_ErrorData struct for more information
    printf("%s", errorData.errorText.chars);
    /*switch(errorData.errorType) {
        // etc
    }*/
}

/*-------------------------------------------------------------------------------------------------
    Button interaction
-------------------------------------------------------------------------------------------------*/

//sidebar buttons
// uint32_t SelectedUserIndex = 0;

//Clay_OnHover(HandleSidebarInteraction, i)
// void HandleSidebarInteraction(Clay_ElementId elementId, Clay_PointerData pointerData, intptr_t userData){
//     if (pointerData.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME){
//         if (userData >= 0 && userData < user_arr.NumUsers){
//             //select corresponding user
//             SelectedUserIndex = userData;
//         }
//     }
// };

/*-------------------------------------------------------------------------------------------------
    Render buttons
-------------------------------------------------------------------------------------------------*/

// void RenderSidebarButton(Clay_String text, int index) {
//     // Create unique ID for each button
//     Clay_ElementId personId = Clay__HashString(text, index, 0);
    
//     CLAY({
//         .id = personId,
//         .layout = { 
//             .sizing = { 
//                 .width = CLAY_SIZING_GROW(0),
//                 .height = CLAY_SIZING_FIXED(60) // Explicit height
//             },
//             .padding = CLAY_PADDING_ALL(16),
//             .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER } 
//         },
//         .backgroundColor = (SelectedUserIndex == index) ? BLUEGRAY : GARNET,
//         .cornerRadius = 8,
//     }) {
//         CLAY_TEXT(text, CLAY_TEXT_CONFIG({ 
//             .fontId = OSWALD, 
//             .fontSize = 40, 
//             .textColor = OLDGOLD
//         }));
//     }
// };

static inline Clay_String buildClayString(const char* string) {
    Clay_String clayString;
    clayString.isStaticallyAllocated = false;
    clayString.length = (int32_t)strlen(string);
    clayString.chars = string;
    return clayString;
}

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
    Render main page and sidebar
-------------------------------------------------------------------------------------------------*/

void renderDirectory(struct directory* dir) {
    int i;
    int pos = 0;
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
    Main Frontend
-------------------------------------------------------------------------------------------------*/

Font* initFrontend() {
    //make window resizable
    int ScreenWidth = 1128;//GetScreenWidth();
    int ScreenHeight = 705;//GetScreenHeight();
    char* ScreenTitle = "Music Manager";
    Clay_Raylib_Initialize(ScreenWidth, ScreenHeight, ScreenTitle, FLAG_WINDOW_RESIZABLE);

    //allocate the memory and pass it in to a clay arena
    uint64_t clayRequiredMemory = Clay_MinMemorySize();
    Clay_Arena clayMemory = (Clay_Arena){
        .memory = malloc(clayRequiredMemory),
        .capacity = clayRequiredMemory
    };
    //initialize the screen
    Clay_Initialize(clayMemory, (Clay_Dimensions) {
        .width = GetScreenWidth(),
        .height = GetScreenHeight(),
    }, (Clay_ErrorHandler) { HandleClayErrors });

    //set up font arrays
    Font* fonts = calloc(4, sizeof(Font));
    fonts[GOTHIC] = LoadFontEx("resources/SpecialGothicExpandedOne-Regular.ttf", 48, 0, 400);
    fonts[JAKARTA] = LoadFontEx("resources/PlusJakartaSans-VariableFont_wght.ttf", 48, 0, 400);
    fonts[PLAYFAIR] = LoadFontEx("resources/PlayfairDisplay-VariableFont_wght.ttf", 48, 0, 400);
    // fonts[OSWALD] = LoadFontEx("resources/Oswald-SemiBold.ttf", 48, 0, 400);
    fonts[OSWALD] = LoadFontEx("resources/CoralPixels-Regular.ttf", 48, 0, 400);

    //render text
    SetTextureFilter(fonts[GOTHIC].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    return fonts;
}

/*-------------------------------------------------------------------------------------------------
    window should update function:
    should eventuall also check:
    if mouse is hovering over key areas
    if window has been resized
    if mouse is scolling
-------------------------------------------------------------------------------------------------*/

int windowShouldUpdate() {
    if (IsMouseButtonDown(0)) {
        return 1;
    }
    return 0;
}

/*-------------------------------------------------------------------------------------------------
    Main function
-------------------------------------------------------------------------------------------------*/

int mainFrontend(Font* fonts, struct directory* dir){
    //run once per frame
    //make the window resizeable
    Clay_SetLayoutDimensions((Clay_Dimensions){
        .width = GetScreenWidth(),
        .height = GetScreenHeight(),
    });

    //mouse input stuff
    Vector2 mousePosition = GetMousePosition();
    //Vector2 scrollDelta = GetMouseWheelMoveV();
    Clay_SetPointerState(
        (Clay_Vector2) { mousePosition.x, mousePosition.y },
        IsMouseButtonDown(0)
    );
    // Clay_UpdateScrollContainers(
    //     true,
    //     (Clay_Vector2) { scrollDelta.x, scrollDelta.y},
    //     GetFrameTime()
    // );

    Clay_BeginLayout();

    //start build ui
    CLAY({
        .id = CLAY_ID("OuterContainer"),
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,  // Stack Header on top of the main row
            .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
            //.padding = CLAY_PADDING_ALL(16),
            .childGap = 10
        },
        .backgroundColor = DARKMODE
    }){
    
        // HEADER
        CLAY({
            .id = CLAY_ID("Header"),
            .layout = {
                .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(100) },  // full width, fixed height
                .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER }
                //.padding = CLAY_PADDING_ALL(12)
            },
            .backgroundColor = GARNET,  // dark grey
        }){
            CLAY_TEXT(CLAY_STRING("Music Library"), CLAY_TEXT_CONFIG({
                .fontId = GOTHIC,
                .fontSize = 65,
                .textColor = BLUEGRAY
            }));
        }
    
        // MAIN ROW: contains sidebar and main content
        CLAY({
            .id = CLAY_ID("MainRow"),
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                .childGap = 10
            },
            //.backgroundColor = { 0, 0, 255, 255 }
        }){
    
            // SIDEBAR
            CLAY({
                .id = CLAY_ID("SideBar"),
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = { .width = CLAY_SIZING_FIXED(225), .height = CLAY_SIZING_GROW(0) },
                    .padding = CLAY_PADDING_ALL(10),
                    .childGap = 10
                },
                .backgroundColor = OLDGOLD
            }){
                //RenderSidebarButton(CLAY_STRING("Add Metadata"), 0);
            }
    
            // MAIN CONTENT (empty for now)
            CLAY({
                .id = CLAY_ID("MainContent"),
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = { CLAY_SIZING_GROW(1), CLAY_SIZING_GROW(0) },
                    .padding = CLAY_PADDING_ALL(10),
                    .childGap = 10
                },
                .backgroundColor = BLUEGRAY  // even darker bg
            }){
                renderDirectory(dir);
            }
    
        }
    }

    // if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
    //     // Check each user button
    //     for (int i = 0; i < user_arr.NumUsers; i++) {
    //         User user = *user_arr.Users[i];
    //         Clay_ElementId btnId = Clay__HashString(user.UserName, i, 0);
    //         if (Clay_PointerOver(btnId)) {
    //             SelectedUserIndex = i;
    //             //printf("Selected user %d\n", i);
    //             break;
    //         }
    //     }
    // };

    Clay_RenderCommandArray renderCommands = Clay_EndLayout();

    //now pass everything to raylib
    BeginDrawing();
    ClearBackground(BLACK);
    Clay_Raylib_Render(renderCommands, fonts);
    EndDrawing();
    return 0;
}