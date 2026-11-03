#ifndef RENDERER_H
#define RENDERER_H

#include "clay.h"      // declarations only - CLAY_IMPLEMENTATION lives in main.cpp
#include "raylib.h"

// Icon ids passed to the renderer through .custom.customData
enum Icon : int { ICON_PREV = 1, ICON_PLAY, ICON_PAUSE, ICON_NEXT };
inline int iconPrev = ICON_PREV, iconPlay = ICON_PLAY, iconPause = ICON_PAUSE, iconNext = ICON_NEXT;

// Tells Clay how wide a piece of text is (userData = pointer to the Font)
Clay_Dimensions MeasureTextCustom(Clay_StringSlice text, Clay_TextElementConfig* config, void* userData);

// Draws one whole frame: clears the screen and renders every Clay command with raylib
void RenderClayCommands(Clay_RenderCommandArray commands, const Font& font);

#endif // RENDERER_H
