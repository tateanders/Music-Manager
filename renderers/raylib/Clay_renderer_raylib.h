#ifndef CLAY_RENDERER_RAYLIB_H
#define CLAY_RENDERER_RAYLIB_H

#include "raylib.h"
#include "raymath.h"
#include "clay.h"

// Expose the shared camera
extern Camera Raylib_camera;

// Custom layout element for 3D models
typedef enum {
    CUSTOM_LAYOUT_ELEMENT_TYPE_3D_MODEL
} CustomLayoutElementType;

typedef struct {
    Model model;
    float scale;
    Vector3 position;
    Matrix rotation;
} CustomLayoutElement_3DModel;

typedef struct {
    CustomLayoutElementType type;
    union {
        CustomLayoutElement_3DModel model;
    } customData;
} CustomLayoutElement;

// Convert Clay types to Raylib types
#define CLAY_RECTANGLE_TO_RAYLIB_RECTANGLE(rect) \
    (Rectangle){ .x = (rect).x, .y = (rect).y, .width = (rect).width, .height = (rect).height }
#define CLAY_COLOR_TO_RAYLIB_COLOR(c) \
    (Color){ .r = (unsigned char)roundf((c).r), \
             .g = (unsigned char)roundf((c).g), \
             .b = (unsigned char)roundf((c).b), \
             .a = (unsigned char)roundf((c).a) }

// Utility: unproject screen point to world ray
Ray GetScreenToWorldPointWithZDistance(Vector2 position,
                                       Camera camera,
                                       int screenWidth,
                                       int screenHeight,
                                       float zDistance);

// Custom text measurement callback
Clay_Dimensions Raylib_MeasureText(Clay_StringSlice text,
                                   Clay_TextElementConfig *config,
                                   void *userData);

// Raylib renderer lifecycle
void Clay_Raylib_Initialize(int width, int height, const char *title, unsigned int flags);
void Clay_Raylib_Close(void);

// Main render entrypoint: draw Clay commands using Raylib
void Clay_Raylib_Render(Clay_RenderCommandArray renderCommands, Font *fonts);

#endif // CLAY_RENDERER_RAYLIB_H