#ifndef AUDIO_H
#define AUDIO_H

#include <string>

void InitGameAudio();
void LoadSong(const std::string& filePath);
void TogglePlayPause();
void UpdateGameAudio();
void StopAndCleanupAudio();

#endif // AUDIO_H