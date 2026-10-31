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

    // تنسيق المدة (MM:SS أو H:MM:SS إذا تجاوزت ساعة)
    std::string getFormattedDuration() const {
        int hours = durationInSeconds / 3600;
        int minutes = (durationInSeconds % 3600) / 60;
        int seconds = durationInSeconds % 60;
        char buffer[24];
        if (hours > 0)
            std::snprintf(buffer, sizeof(buffer), "%d:%02d:%02d", hours, minutes, seconds);
        else
            std::snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, seconds);
        return std::string(buffer);
    }
};

#endif // SONG_H