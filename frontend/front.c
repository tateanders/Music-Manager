#define CLAY_IMPLEMENTATION
#include "front.h"
#include "clay_renderer_raylib.c"
#include "components.h"
#include <inttypes.h>

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
    free
-------------------------------------------------------------------------------------------------*/

void freeFonts(Font* fonts) {
    UnloadFont(fonts[GOTHIC]);
    UnloadFont(fonts[JAKARTA]);
    UnloadFont(fonts[PLAYFAIR]);
    UnloadFont(fonts[OSWALD]);
    free(fonts);
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

struct directory* mainFrontend(Font* fonts, struct directory* dir, int goBack, int* tagsAdded){
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
        renderHeader(goBack);
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
                renderSidebarButton(*tagsAdded);
            }
    
            // MAIN CONTENT (empty for now)
            CLAY({
                .id = CLAY_ID("MainContent"),
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = { CLAY_SIZING_GROW(1), CLAY_SIZING_GROW(0) },
                    .padding = CLAY_PADDING_ALL(10),
                    .childGap = 10,
                },
                .backgroundColor = BLUEGRAY  // even darker bg
            }){
                if(dir) {
                    renderDirectory(dir);
                } else {
                    renderDirHeader2();
                }
            }
    
        }
    }

    if (dir && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && dir->directories){
        // Check each user button
        for (int i = 0; i < list_getNumElements(dir->directories); i++) {
            struct directory* tempDir = list_getElement(dir->directories, i);
            Clay_String dirName = buildClayString(tempDir->dirName);
            Clay_ElementId btnId = Clay__HashString(dirName, i, 0);
            if (Clay_PointerOver(btnId)) {
                dir = tempDir;
                break;
            }
        }
    };

    Clay_RenderCommandArray renderCommands = Clay_EndLayout();

    //now pass everything to raylib
    BeginDrawing();
    ClearBackground(BLACK);
    Clay_Raylib_Render(renderCommands, fonts);
    EndDrawing();

    //check if the back button or add tags was pressed
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        Clay_ElementId bId = Clay__HashString(CLAY_STRING("Back Button Container"), 0, 0);
        if (Clay_PointerOver(bId)) {
            return NULL;
        }
        bId = Clay__HashString(CLAY_STRING("Add Tags"), 0, 0);
        if (Clay_PointerOver(bId)) {
            *tagsAdded = 1;
        }
    }

    return dir;
}