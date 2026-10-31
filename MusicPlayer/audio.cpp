#include "audio.h"
#include "raylib.h"

static Music music;
static bool isMusicLoaded = false;
static bool isPaused = false;

void InitGameAudio() {
    InitAudioDevice();
}

void UnloadSong() {
    if (isMusicLoaded) {
        StopMusicStream(music);
        UnloadMusicStream(music);
        isMusicLoaded = false;
    }
    isPaused = false;
}

void LoadSong(const std::string& filePath) {
    UnloadSong();

    music = LoadMusicStream(filePath.c_str());
    if (music.stream.buffer != NULL) {
        music.looping = false;          // so we can detect the end and go to the next song
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

void RestartSong() {
    if (!isMusicLoaded) return;
    SeekMusicStream(music, 0.0f);
}

void UpdateGameAudio() {
    if (isMusicLoaded && !isPaused) {
        UpdateMusicStream(music);
    }
}

bool IsSongLoaded() {
    return isMusicLoaded;
}

bool IsSongFinished() {
    // With looping disabled raylib stops the stream when the song ends.
    return isMusicLoaded && !isPaused && !IsMusicStreamPlaying(music);
}

float GetSongTimePlayed() {
    return isMusicLoaded ? GetMusicTimePlayed(music) : 0.0f;
}

int GetAudioFileDuration(const std::string& filePath) {
    Music tmp = LoadMusicStream(filePath.c_str());
    if (tmp.stream.buffer == NULL) return 0;
    float len = GetMusicTimeLength(tmp);
    UnloadMusicStream(tmp);
    return static_cast<int>(len + 0.5f);
}

void StopAndCleanupAudio() {
    UnloadSong();
    CloseAudioDevice();
}