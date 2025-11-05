#define CLAY_IMPLEMENTATION
#include "front.h"
#include "clay_renderer_raylib.c"
#include "components.h"
#include "GOTHIC.c"
#include "NOTO.c"
#include "NOTOI.c"

/*-------------------------------------------------------------------------------------------------
    Error handler
-------------------------------------------------------------------------------------------------*/

void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s", errorData.errorText.chars);
}

/*-------------------------------------------------------------------------------------------------
    free
-------------------------------------------------------------------------------------------------*/

void freeFonts(Font* fonts) {
    UnloadFont(fonts[GOTHIC]);
    UnloadFont(fonts[NOTO]);
    UnloadFont(fonts[NOTOI]);
    free(fonts);
}

/*-------------------------------------------------------------------------------------------------
    Main Frontend
-------------------------------------------------------------------------------------------------*/

Font* initFrontend() {
    //make window resizable
    int ScreenWidth = 1128;
    int ScreenHeight = 705;
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
    Font* fonts = calloc(3, sizeof(Font));
    fonts[GOTHIC] = LoadFontFromMemory(".ttf", SpecialGothicExpandedOne, SpecialGothicExpandedOne_len, 48, 0, 0);
    SetTextureFilter(fonts[GOTHIC].texture, TEXTURE_FILTER_BILINEAR);
    fonts[NOTO] = LoadFontFromMemory(".ttf", NotoSans_Bold_ttf, NotoSans_Bold_ttf_len, 48, 0, 0);
    SetTextureFilter(fonts[NOTO].texture, TEXTURE_FILTER_BILINEAR);
    fonts[NOTOI] = LoadFontFromMemory(".ttf", NotoSans_BoldItalic_ttf, NotoSans_BoldItalic_ttf_len, 48, 0, 0);
    SetTextureFilter(fonts[NOTOI].texture, TEXTURE_FILTER_BILINEAR);

    //render text
    SetTextureFilter(fonts[GOTHIC].texture, TEXTURE_FILTER_BILINEAR);
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    return fonts;
}

/*-------------------------------------------------------------------------------------------------
    Main function
-------------------------------------------------------------------------------------------------*/
struct dataToShow* mainFrontend(struct dataToShow* data, struct list* backList) {
    //run once per frame
    //make the window resizeable
    Clay_SetLayoutDimensions((Clay_Dimensions){
        .width = GetScreenWidth(),
        .height = GetScreenHeight(),
    });

    //mouse input stuff
    Vector2 mousePosition = GetMousePosition();
    Vector2 scrollDelta = GetMouseWheelMoveV();
    Clay_SetPointerState(
        (Clay_Vector2) { mousePosition.x, mousePosition.y },
        IsMouseButtonDown(0)
    );
    Clay_UpdateScrollContainers(
        true,
        (Clay_Vector2) { scrollDelta.x, scrollDelta.y },
        GetFrameTime()
    );

    Clay_BeginLayout();

    //start build ui
    CLAY( CLAY_ID("OuterContainer"), {
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
            .childGap = 0
        },
        .backgroundColor = _BLACK
    }){
        renderHeader(list_getNumElements(backList));
        // MAIN ROW: contains sidebar and main content
        CLAY( CLAY_ID("MainRow"), {
            .layout = {
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .sizing = { CLAY_SIZING_PERCENT(1), CLAY_SIZING_GROW(0) },
                .childGap = 0
            },
        }){
    
            // SIDEBAR
            CLAY( CLAY_ID("SideBar"), {
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = { .width = CLAY_SIZING_PERCENT(.20), .height = CLAY_SIZING_GROW(0) },
                    .padding = CLAY_PADDING_ALL(10),
                    .childGap = 10
                },
                .backgroundColor = _LESSBLACK
            }){
                renderSidebarButtons(data->info[TAGS], data->info[FINDDUPS]);
            }
    
            // MAIN CONTENT (empty for now)
            CLAY( CLAY_ID("MainContent"), {
                .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() },
                .layout = {
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = { CLAY_SIZING_PERCENT(.80), CLAY_SIZING_GROW(0) },
                    .padding = CLAY_PADDING_ALL(10),
                    .childGap = 10,
                },
                .backgroundColor = _BLACK,
            }){
                if(data->info[TAGS] || data->info[FINDDUPS]) {
                    renderDirHeader2();
                } else if (data->info[SHOWDUPS]) {
                    renderDups(data->duplicates);
                } else {
                    renderDirectory(data->dir);
                }
            }
    
        }
    }

    Clay_RenderCommandArray renderCommands;
    renderCommands = Clay_EndLayout();

    //now pass everything to raylib
    BeginDrawing();
    ClearBackground(BLACK);
    Clay_Raylib_Render(renderCommands, data->fonts);
    EndDrawing();

    //check if the back button or sidebar buttons
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        Clay_ElementId bId = Clay__HashString(CLAY_STRING("Back Button Container"), 0);
        if (Clay_PointerOver(bId)) {
            data->info[BACK] = 1;
            if (data->duplicates) {
                freeDups(data->duplicates);
                data->duplicates = NULL;
                data->info[SHOWDUPS] = 0;
            }
        }
        bId = Clay__HashString(CLAY_STRING("Add Tags"), 0);
        if (Clay_PointerOver(bId)) {
            data->info[TAGS] = 1;
            if (data->duplicates) {
                freeDups(data->duplicates);
                data->duplicates = NULL;
                data->info[SHOWDUPS] = 0;
            }
        }
        bId = Clay__HashString(CLAY_STRING("Find Dups"), 0);
        if (Clay_PointerOver(bId)) {
            data->info[FINDDUPS] = 1;
        }
    }
    // Check if a user button was pushed
    if (data->dir && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && data->dir->directories){
        for (int i = 0; i < list_getNumElements(data->dir->directories); i++) {
            struct directory* tempDir = list_getElement(data->dir->directories, i);
            Clay_String dirName = buildClayString(tempDir->dirName);
            Clay_ElementId btnId = Clay__HashString(dirName, i);
            if (Clay_PointerOver(btnId)) {
                list_insert(backList, data->dir);
                data->dir = tempDir;
                break;
            }
        }
    };

    return data;
}