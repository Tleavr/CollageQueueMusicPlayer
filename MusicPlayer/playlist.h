#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <queue>
#include <string>
#include <vector>
#include "song.h"

// Queue (FIFO) based playlist.
//  - songQueue   : songs waiting to be played (front = next song)
//  - nowPlaying  : the song that was dequeued and is playing right now
class Playlist {
private:
    std::queue<Song> songQueue;
    Song nowPlaying;
    bool hasNowPlaying = false;

public:
    // 1. Add Song      : enqueue at the end
    void addSong(const Song& song);
    // 2. Play Next     : dequeue the front song and make it the playing song
    //                    returns false (and nothing is playing) if the queue is empty
    bool nextSong();
    // 3. View Next     : peek at the front of the queue without removing it
    Song peekNextSong() const;
    bool hasNextSong() const;

    Song getCurrentSong() const;
    bool hasCurrentSong() const;

    // 4. Display       : playing song first, then the queue in order
    std::vector<Song> getAllSongs() const;
    // 5. Clear
    void clearPlaylist();
    // 6. Statistics    : count and duration of everything in the playlist
    size_t getSongCount() const;
    int getTotalDurationSeconds() const;
    std::string getFormattedTotalDuration() const;   // HH:MM:SS
    bool isEmpty() const;

    // Saving / loading (one song per line in a plain text file)
    bool saveToFile(const std::string& filePath) const;
    void loadFromFile(const std::string& filePath);  // skips songs whose file no longer exists
};

#endif // PLAYLIST_H
