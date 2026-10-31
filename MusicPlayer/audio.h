#ifndef AUDIO_H
#define AUDIO_H

#include <string>

void  InitGameAudio();
void  LoadSong(const std::string& filePath);   // loads + starts playing
void  UnloadSong();                            // stops and frees the current song
void  TogglePlayPause();
void  RestartSong();                           // jump back to 00:00
void  UpdateGameAudio();
void  StopAndCleanupAudio();

bool  IsSongLoaded();
bool  IsSongFinished();                        // true once the song reached its end
float GetSongTimePlayed();                     // seconds
int   GetAudioFileDuration(const std::string& filePath); // seconds, 0 if file can't be opened

#endif // AUDIO_H
