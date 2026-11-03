#include "renderer.h"

#include <string>

// ---------------- helpers ----------------

Clay_Dimensions MeasureTextCustom(Clay_StringSlice text, Clay_TextElementConfig* config, void* userData)
{
    Font* font = static_cast<Font*>(userData);
    std::string value(text.chars, text.length);
    Vector2 size = MeasureTextEx(*font, value.c_str(),
        static_cast<float>(config->fontSize), static_cast<float>(config->letterSpacing));
    return Clay_Dimensions{ size.x, size.y };
}

// Draws the play / pause / prev / next glyphs inside a Clay bounding box
static void DrawIcon(int icon, Clay_BoundingBox b)
{
    const float s = (b.width < b.height ? b.width : b.height);
    const float cx = b.x + b.width / 2, cy = b.y + b.height / 2;
    const Color c = { 244, 245, 248, 255 };
    const float h = s * 0.5f, w = s * 0.45f;   // triangle size
    const float o = s * 0.04f, bar = s * 0.07f; // small shift / bar thickness

    switch (icon) {
    case ICON_PLAY:
        DrawTriangle({ cx - w / 2 + o, cy - h / 2 }, { cx - w / 2 + o, cy + h / 2 }, { cx + w / 2 + o, cy }, c);
        break;
    case ICON_PAUSE:
        DrawRectangleRec({ cx - h * 0.3f, cy - h / 2, h * 0.2f, h }, c);
        DrawRectangleRec({ cx + h * 0.1f, cy - h / 2, h * 0.2f, h }, c);
        break;
    case ICON_NEXT:   // |▶  mirrored: ▶|
        DrawTriangle({ cx - w / 2 - o, cy - h / 2 }, { cx - w / 2 - o, cy + h / 2 }, { cx + w / 2 - o, cy }, c);
        DrawRectangleRec({ cx + w / 2 - o, cy - h / 2, bar, h }, c);
        break;
    case ICON_PREV:   // |◀
        DrawTriangle({ cx + w / 2 + o, cy - h / 2 }, { cx - w / 2 + o, cy }, { cx + w / 2 + o, cy + h / 2 }, c);
        DrawRectangleRec({ cx - w / 2 - o, cy - h / 2, bar, h }, c);
        break;
    }
}

static float RoundnessFor(float radius, const Clay_BoundingBox& b)
{
    // raylib: radius = min(w,h) * roundness / 2
    const float m = (b.width < b.height ? b.width : b.height);
    if (m <= 0.0f || radius <= 0.0f) return 0.0f;
    float r = radius * 2.0f / m;
    return r > 1.0f ? 1.0f : r;
}

static Color ToColor(Clay_Color c)
{
    return Color{ (unsigned char)c.r, (unsigned char)c.g, (unsigned char)c.b, (unsigned char)c.a };
}

// ---------------- Raylib renderer ----------------
void RenderClayCommands(Clay_RenderCommandArray commands, const Font& font)
{
    BeginDrawing();
    ClearBackground({ 15, 17, 21, 255 });

    for (int32_t i = 0; i < commands.length; ++i)
    {
        Clay_RenderCommand* cmd = Clay_RenderCommandArray_Get(&commands, i);
        const Clay_BoundingBox bb = cmd->boundingBox;
        const Rectangle rec = { bb.x, bb.y, bb.width, bb.height };

        switch (cmd->commandType)
        {
        case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
            auto* d = &cmd->renderData.rectangle;
            if (d->cornerRadius.topLeft > 0.0f)
                DrawRectangleRounded(rec, RoundnessFor(d->cornerRadius.topLeft, bb), 24, ToColor(d->backgroundColor));
            else
                DrawRectangleRec(rec, ToColor(d->backgroundColor));
            break;
        }
        case CLAY_RENDER_COMMAND_TYPE_BORDER: {
            auto* d = &cmd->renderData.border;
            DrawRectangleRoundedLinesEx(rec, RoundnessFor(d->cornerRadius.topLeft, bb), 24,
                (float)d->width.left, ToColor(d->color));
            break;
        }
        case CLAY_RENDER_COMMAND_TYPE_TEXT: {
            auto* d = &cmd->renderData.text;
            std::string value(d->stringContents.chars, d->stringContents.length);
            DrawTextEx(font, value.c_str(), { bb.x, bb.y },
                (float)d->fontSize, (float)d->letterSpacing, ToColor(d->textColor));
            break;
        }
        case CLAY_RENDER_COMMAND_TYPE_SCISSOR_START:
            BeginScissorMode((int)bb.x, (int)bb.y, (int)bb.width, (int)bb.height);
            break;
        case CLAY_RENDER_COMMAND_TYPE_SCISSOR_END:
            EndScissorMode();
            break;
        case CLAY_RENDER_COMMAND_TYPE_CUSTOM: {
            auto* d = &cmd->renderData.custom;
            if (d->customData) DrawIcon(*static_cast<int*>(d->customData), bb);
            break;
        }
        default: break;
        }
    }

    EndDrawing();
}