#include "audio.h"
#include "raylib.h"

static Music music;
static bool isMusicLoaded = false;
static bool isPaused = false;

void InitGameAudio() {
    InitAudioDevice();
}

void LoadSong(const std::string& filePath) {
    if (isMusicLoaded) {
        StopMusicStream(music);
        UnloadMusicStream(music);
        isMusicLoaded = false;
    }

    music = LoadMusicStream(filePath.c_str());
    if (music.stream.buffer != NULL) {
        PlayMusicStream(music);
        isMusicLoaded = true;
        isPaused = false;
    }
}

void TogglePlayPause() {
    if (!isMusicLoaded) return;

    if (isPaused) {
        ResumeMusicStream(music);
        isPaused = false;
    }
    else {
        PauseMusicStream(music);
        isPaused = true;
    }
}

void UpdateGameAudio() {
    if (isMusicLoaded && !isPaused) {
        UpdateMusicStream(music);
    }
}

void StopAndCleanupAudio() {
    if (isMusicLoaded) {
        StopMusicStream(music);
        UnloadMusicStream(music);
        isMusicLoaded = false;
    }
    CloseAudioDevice();
}