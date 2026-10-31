#include "playlist.h"
#include <cstdio>

// 1. Add Song
void Playlist::addSong(const Song& song) {
    songQueue.push(song);
}

// 2. Play Next (حذف الحالية من المقدمة)
void Playlist::nextSong() {
    if (!songQueue.empty()) {
        songQueue.pop();
    }
}

// 3. View Next Song (Peek)
Song Playlist::peekNextSong() const {
    if (!songQueue.empty()) {
        return songQueue.front();
    }
    return Song("No Song", "Unknown", "", 0);
}

Song Playlist::getCurrentSong() const {
    return peekNextSong();
}

// 4. Display Playlist (نسخ الطابور إلى Vector للعرض دون إتلاف الطابور الأصلي)
std::vector<Song> Playlist::getAllSongs() const {
    std::vector<Song> songsList;
    std::queue<Song> tempQueue = songQueue;
    while (!tempQueue.empty()) {
        songsList.push_back(tempQueue.front());
        tempQueue.pop();
    }
    return songsList;
}

// 5. Clear Playlist
void Playlist::clearPlaylist() {
    while (!songQueue.empty()) {
        songQueue.pop();
    }
}

// 6. Playlist Statistics (الإحصائيات)
size_t Playlist::getSongCount() const {
    return songQueue.size();
}

int Playlist::getTotalDurationSeconds() const {
    int total = 0;
    std::queue<Song> tempQueue = songQueue;
    while (!tempQueue.empty()) {
        total += tempQueue.front().durationInSeconds;
        tempQueue.pop();
    }
    return total;
}

std::string Playlist::getFormattedTotalDuration() const {
    int totalSecs = getTotalDurationSeconds();
    int minutes = totalSecs / 60;
    int seconds = totalSecs % 60;
    char buffer[15];
    std::snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);
    return std::string(buffer);
}

bool Playlist::isEmpty() const {
    return songQueue.empty();
}