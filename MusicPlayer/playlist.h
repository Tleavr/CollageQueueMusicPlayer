#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <queue>
#include <string>
#include <vector>
#include "song.h"

class Playlist {
private:
    std::queue<Song> songQueue; // طابور الأغاني (FIFO)

public:
    void addSong(const Song& song);
    void nextSong();
    Song peekNextSong() const;
    Song getCurrentSong() const;
    std::vector<Song> getAllSongs() const;
    void clearPlaylist();
    size_t getSongCount() const;
    int getTotalDurationSeconds() const;
    std::string getFormattedTotalDuration() const;
    bool isEmpty() const;
};

#endif // PLAYLIST_H