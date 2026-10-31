#pragma once
#include <string>

void InitGameAudio();
void LoadSong(const std::string& filePath);
void TogglePlayPause();
void UpdateGameAudio();
void StopAndCleanupAudio();