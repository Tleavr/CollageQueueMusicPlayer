#define CLAY_IMPLEMENTATION
#include "clay.h"

#include "raylib.h"
#include "song.h"
#include "audio.h"
#include "playlist.h"
#include "file_dialog.h"
#include "renderer.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <deque>
#include <string>
#include <vector>

// -----------------------------
// Window / theme configuration
// -----------------------------
static constexpr int SCREEN_WIDTH = 810;
static constexpr int SCREEN_HEIGHT = 530;

static const Clay_Color UI_BG = { 18, 18, 18, 250 };
static const Clay_Color UI_PANEL = { 43, 46, 54, 255 };
static const Clay_Color UI_ROW_ACTIVE = { 62, 67, 80, 255 };
static const Clay_Color UI_BORDER = { 133, 139, 164, 255 };
static const Clay_Color UI_WHITE = { 244, 245, 248, 255 };
static const Clay_Color UI_MUTED = { 184, 188, 201, 255 };
static const Clay_Color UI_ACCENT = { 155, 164, 205, 255 };
static const Clay_Color UI_HOVER = { 74, 80, 96, 255 };
static const Clay_Color UI_NONE = { 0, 0, 0, 0 };

// Clay does NOT copy text, so every string must stay alive until Clay_EndLayout().
// A deque never invalidates references when we push_back, so it is safe to hand out pointers.
static std::deque<std::string> g_frameStrings;

static Clay_String FrameStr(const std::string& value)
{
    g_frameStrings.push_back(value);
    const std::string& s = g_frameStrings.back();
    return Clay_String{ false, static_cast<int32_t>(s.size()), s.c_str() };
}

static Clay_String ClayStr(const char* value)   // string literals only
{
    int32_t length = 0;
    while (value && value[length] != '\0') ++length;
    return Clay_String{ true, length, value };
}

// One cell of the table (header or row): same GROW(1) width everywhere so columns line up
static void Cell(Clay_String text, Clay_TextElementConfig style)
{
    CLAY_AUTO_ID({
        .layout = {
            .sizing = { CLAY_SIZING_GROW(1), CLAY_SIZING_FIT() },
            .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER }
        },
        .clip = { .horizontal = true }
        }) {
        CLAY_TEXT(text, style);
    }
}

int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Music Player");
    SetWindowMinSize(640, 420);
    SetTargetFPS(60);

    InitGameAudio();
    Playlist playlist;

    // The playlist is saved next to the .exe, and loaded again when the app starts
    const std::string savePath = std::string(GetApplicationDirectory()) + "playlist.txt";
    playlist.loadFromFile(savePath);

    // Load at a large size and scale down with bilinear filtering = smooth text
    Font customFont = LoadFontEx("assets/FredokaOne-Regular.ttf", 96, nullptr, 0);
    if (customFont.texture.id == 0) customFont = GetFontDefault();
    else { GenTextureMipmaps(&customFont.texture); SetTextureFilter(customFont.texture, TEXTURE_FILTER_TRILINEAR); }

    const uint64_t clayMemorySize = Clay_MinMemorySize();
    void* clayBuffer = std::malloc(clayMemorySize);
    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(clayMemorySize, clayBuffer);
    Clay_ErrorHandler errorHandler = { nullptr, nullptr };
    Clay_Initialize(clayMemory,
        Clay_Dimensions{ (float)GetScreenWidth(), (float)GetScreenHeight() }, errorHandler);
    Clay_SetMeasureTextFunction(MeasureTextCustom, &customFont);

    bool isPlaying = false;

    auto save = [&]() { playlist.saveToFile(savePath); };

    // Start playing the current song (or stop if the playlist is empty)
    auto playCurrent = [&]() {
        if (!playlist.hasCurrentSong()) { UnloadSong(); isPlaying = false; return; }
        LoadSong(playlist.getCurrentSong().filePath);
        isPlaying = IsSongLoaded();
        };
    // Play Next = dequeue the front song and play it (the old song leaves the list)
    auto goNext = [&]() {
        playlist.nextSong();
        playCurrent();
        save();
        };

    while (!WindowShouldClose())
    {
        UpdateGameAudio();
        if (IsSongFinished()) goNext();      // auto-advance

        g_frameStrings.clear();

        // ---- input state must be given to Clay BEFORE the layout starts ----
        Vector2 mouse = GetMousePosition();
        Vector2 wheel = GetMouseWheelMoveV();
        Clay_SetPointerState({ mouse.x, mouse.y }, IsMouseButtonDown(MOUSE_BUTTON_LEFT));
        Clay_UpdateScrollContainers(false, { wheel.x * 30.0f, wheel.y * 30.0f }, GetFrameTime());
        Clay_SetLayoutDimensions({ (float)GetScreenWidth(), (float)GetScreenHeight() });

        const bool click = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        const bool clearHovered = Clay_PointerOver(CLAY_ID("ClearPlaylistBtn"));
        const bool addHovered = Clay_PointerOver(CLAY_ID("AddSongBtn"));
        const bool prevHovered = Clay_PointerOver(CLAY_ID("PrevBtn"));
        const bool playHovered = Clay_PointerOver(CLAY_ID("PlayPauseBtn"));
        const bool nextHovered = Clay_PointerOver(CLAY_ID("NextBtn"));

        if (click && clearHovered) { playlist.clearPlaylist(); UnloadSong(); isPlaying = false; save(); }

        if (click && addHovered) {
            std::string path = OpenMusicFileDialog();
            if (!path.empty()) {
                int duration = GetAudioFileDuration(path);
                if (duration > 0) {
                    bool wasEmpty = playlist.isEmpty();
                    playlist.addSong(MakeSongFromPath(path, duration));
                    save();
                    if (wasEmpty) goNext();          // first song starts automatically
                }
            }
        }

        if (click && playHovered) {
            if (!IsSongLoaded()) goNext();           // nothing playing yet -> dequeue the first song and play it
            else { TogglePlayPause(); isPlaying = !isPlaying; }
        }
        if (click && nextHovered) goNext();
        if (click && prevHovered) RestartSong();     // a queue can't go back, so Previous restarts the song
        if (IsKeyPressed(KEY_F11)) ToggleBorderlessWindowed();

        // ---- responsive scale: 1.0 at 810x530, grows/shrinks with the window ----
        const float scale = std::fmin((float)GetScreenWidth() / SCREEN_WIDTH,
            (float)GetScreenHeight() / SCREEN_HEIGHT);
        const float k = scale < 0.75f ? 0.75f : (scale > 3.5f ? 3.5f : scale);
        auto U = [&](float v) { return static_cast<uint16_t>(v * k + 0.5f); };   // for uint16 fields
        auto F = [&](float v) { return v * k; };                                  // for float fields
        const uint16_t B = U(3);   // border thickness

        // ---- text styles ----
        auto Style = [&](Clay_Color c, float size) {
            return Clay_TextElementConfig{ .textColor = c, .fontId = 0, .fontSize = U(size), .wrapMode = CLAY_TEXT_WRAP_NONE };
            };
        Clay_TextElementConfig titleStyle = Style(UI_WHITE, 26);
        Clay_TextElementConfig headerStyle = Style(UI_WHITE, 22);
        Clay_TextElementConfig bodyStyle = Style(UI_MUTED, 20);
        Clay_TextElementConfig activeStyle = Style(UI_WHITE, 20);
        Clay_TextElementConfig smallStyle = Style(UI_WHITE, 18);
        Clay_TextElementConfig labelStyle = Style(UI_WHITE, 15);
        Clay_TextElementConfig nextStyle = Style(UI_MUTED, 15);
        Clay_TextElementConfig timeStyle = Style(UI_WHITE, 26);

        std::vector<Song> songs = playlist.getAllSongs();
        const bool hasCurrent = playlist.hasCurrentSong();
        const Song currentSong = playlist.getCurrentSong();
        const bool hasNext = playlist.hasNextSong();
        const Song nextSongInfo = playlist.peekNextSong();

        Clay_BeginLayout();

        // ================= Outer frame =================
        CLAY(CLAY_ID("Frame"), {
            .layout = {
                .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                .padding = { U(22), U(22), U(18), U(18) },
                .childGap = U(10),
                .layoutDirection = CLAY_TOP_TO_BOTTOM
            },
            .backgroundColor = UI_BG,
            .cornerRadius = { F(14), F(14), F(14), F(14) },
            .border = { .color = UI_WHITE, .width = { U(2), U(2), U(2), U(2), 0 } }
            }) {

            // ---------- Top row: Clear / Add ----------
            CLAY_AUTO_ID({
                .layout = {
                    .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIT() },
                    .childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER }
                }
                }) {
                CLAY(CLAY_ID("ClearPlaylistBtn"), {
                    .layout = { .padding = { U(8), U(8), U(4), U(4) } },
                    .backgroundColor = clearHovered ? UI_HOVER : UI_NONE,
                    .cornerRadius = { F(8), F(8), F(8), F(8) }
                    }) {
                    CLAY_TEXT(ClayStr("Clear Playlist x"), titleStyle);
                }
                CLAY_AUTO_ID({ .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(1) } } }) {}
                CLAY(CLAY_ID("AddSongBtn"), {
                    .layout = { .padding = { U(8), U(8), U(4), U(4) } },
                    .backgroundColor = addHovered ? UI_HOVER : UI_NONE,
                    .cornerRadius = { F(8), F(8), F(8), F(8) }
                    }) {
                    CLAY_TEXT(ClayStr("Add song +"), titleStyle);
                }
            }

            // ---------- Playlist panel ----------
            CLAY(CLAY_ID("PlaylistPanel"), {
                .layout = {
                    .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                    .padding = { U(16), U(16), U(10), U(10) },
                    .childGap = U(8),
                    .layoutDirection = CLAY_TOP_TO_BOTTOM
                },
                .backgroundColor = UI_PANEL,
                .cornerRadius = { F(14), F(14), F(14), F(14) },
                .border = { .color = UI_BORDER, .width = { B, B, B, B, 0 } }
                }) {

                // Column headers, each with the little underline bar
                CLAY_AUTO_ID({ .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIT() }, .childGap = U(24) } }) {
                    const char* heads[3] = { "Song Name", "Artist", "Duration" };
                    for (int i = 0; i < 3; ++i) {
                        CLAY_AUTO_ID({
                            .layout = {
                                .sizing = { CLAY_SIZING_GROW(1), CLAY_SIZING_FIT() },
                                .childGap = U(2),
                                .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_TOP },
                                .layoutDirection = CLAY_TOP_TO_BOTTOM
                            }
                            }) {
                            CLAY_TEXT(ClayStr(heads[i]), headerStyle);
                            CLAY_AUTO_ID({
                                .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(F(5)) } },
                                .backgroundColor = UI_BORDER,
                                .cornerRadius = { F(3), F(3), F(3), F(3) }
                                }) {
                            }
                        }
                    }
                }

                // Scrollable list with ALL the songs (click a row to play it)
                CLAY(CLAY_ID("SongList"), {
                    .layout = {
                        .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                        .childGap = U(4),
                        .layoutDirection = CLAY_TOP_TO_BOTTOM
                    },
                    .clip = { .vertical = true, .childOffset = Clay_GetScrollOffset() }
                    }) {
                    for (size_t i = 0; i < songs.size(); ++i) {
                        const bool active = (i == 0 && hasCurrent);   // first row = the song playing now
                        CLAY_AUTO_ID({
                            .layout = {
                                .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIT() },
                                .padding = { 0, 0, U(6), U(6) },
                                .childGap = U(24),
                                .childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER }
                            },
                            .backgroundColor = active ? UI_ROW_ACTIVE : UI_NONE,
                            .cornerRadius = { F(8), F(8), F(8), F(8) }
                            }) {
                            Cell(FrameStr(songs[i].title), active ? activeStyle : bodyStyle);
                            Cell(FrameStr(songs[i].artist), active ? activeStyle : bodyStyle);
                            Cell(FrameStr(songs[i].getFormattedDuration()), active ? activeStyle : bodyStyle);
                        }
                    }
                }
            }

            // ---------- Stats row ----------
            CLAY_AUTO_ID({
                .layout = {
                    .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIT() },
                    .padding = { U(6), U(6), 0, 0 },
                    .childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER }
                }
                }) {
                CLAY_TEXT(FrameStr("Total songs : " + std::to_string(playlist.getSongCount())), smallStyle);
                CLAY_AUTO_ID({ .layout = { .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(1) } } }) {}
                CLAY_TEXT(FrameStr("Total Duration : " + playlist.getFormattedTotalDuration()), smallStyle);
            }

            // ---------- Player panel ----------
            CLAY(CLAY_ID("PlayerPanel"), {
                .layout = {
                    .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIXED(F(84)) },
                    .padding = { U(20), U(20), U(10), U(10) },
                    .childGap = U(12),
                    .childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_CENTER }
                },
                .backgroundColor = UI_PANEL,
                .cornerRadius = { F(14), F(14), F(14), F(14) },
                .border = { .color = UI_BORDER, .width = { B, B, B, B, 0 } }
                }) {

                // Current song name + "Next:" line (left)
                CLAY_AUTO_ID({
                    .layout = {
                        .sizing = { CLAY_SIZING_GROW(1), CLAY_SIZING_FIT() },
                        .childGap = U(2),
                        .layoutDirection = CLAY_TOP_TO_BOTTOM
                    },
                    .clip = { .horizontal = true }
                    }) {
                    CLAY_TEXT(hasCurrent ? FrameStr(currentSong.title) : ClayStr("Song Name"), headerStyle);
                    CLAY_TEXT(FrameStr("Next: " + (hasNext ? nextSongInfo.title : std::string("none"))), nextStyle);
                }

                // Prev / Play / Next (center)
                CLAY_AUTO_ID({
                    .layout = {
                        .sizing = { CLAY_SIZING_FIT(), CLAY_SIZING_FIT() },
                        .childGap = U(22),
                        .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER }
                    }
                    }) {
                    CLAY(CLAY_ID("PrevBtn"), {
                        .layout = { .sizing = { CLAY_SIZING_FIXED(F(52)), CLAY_SIZING_FIXED(F(52)) } },
                        .backgroundColor = prevHovered ? UI_HOVER : UI_NONE,
                        .cornerRadius = { F(26), F(26), F(26), F(26) },
                        .custom = { &iconPrev },
                        .border = { .color = UI_WHITE, .width = { B, B, B, B, 0 } }
                        }) {
                    }
                    CLAY(CLAY_ID("PlayPauseBtn"), {
                        .layout = { .sizing = { CLAY_SIZING_FIXED(F(60)), CLAY_SIZING_FIXED(F(60)) } },
                        .backgroundColor = playHovered ? UI_HOVER : UI_NONE,
                        .cornerRadius = { F(30), F(30), F(30), F(30) },
                        .custom = { isPlaying ? &iconPause : &iconPlay },
                        .border = { .color = UI_WHITE, .width = { B, B, B, B, 0 } }
                        }) {
                    }
                    CLAY(CLAY_ID("NextBtn"), {
                        .layout = { .sizing = { CLAY_SIZING_FIXED(F(52)), CLAY_SIZING_FIXED(F(52)) } },
                        .backgroundColor = nextHovered ? UI_HOVER : UI_NONE,
                        .cornerRadius = { F(26), F(26), F(26), F(26) },
                        .custom = { &iconNext },
                        .border = { .color = UI_WHITE, .width = { B, B, B, B, 0 } }
                        }) {
                    }
                }

                // Duration of the current song (right)
                CLAY_AUTO_ID({
                    .layout = {
                        .sizing = { CLAY_SIZING_GROW(1), CLAY_SIZING_FIT() },
                        .childAlignment = { CLAY_ALIGN_X_RIGHT, CLAY_ALIGN_Y_CENTER },
                        .layoutDirection = CLAY_TOP_TO_BOTTOM
                    }
                    }) {
                    CLAY_TEXT(ClayStr("Duration"), labelStyle);
                    CLAY_TEXT(FrameStr(hasCurrent ? currentSong.getFormattedDuration() : std::string("00:00")), timeStyle);
                }
            }
        }

        Clay_RenderCommandArray commands = Clay_EndLayout(GetFrameTime());


        RenderClayCommands(commands, customFont);
    }

    save();
    UnloadFont(customFont);
    StopAndCleanupAudio();
    std::free(clayBuffer);
    CloseWindow();
    return 0;
}