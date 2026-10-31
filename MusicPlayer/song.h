#ifndef SONG_H
#define SONG_H

#include <string>
#include <cstdio>

struct Song {
    std::string title;     // اسم الأغنية
    std::string artist;    // اسم الفنان
    std::string filePath;  // مسار الملف الصوتي
    int durationInSeconds; // مدة الأغنية بالثواني

    Song(const std::string& t = "",
        const std::string& a = "",
        const std::string& path = "",
        int duration = 0)
        : title(t), artist(a), filePath(path), durationInSeconds(duration) {
    }

    // تنسيق المدة (MM:SS)
    std::string getFormattedDuration() const {
        int minutes = durationInSeconds / 60;
        int seconds = durationInSeconds % 60;
        char buffer[10];
        std::snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);
        return std::string(buffer);
    }
};

#endif // SONG_H