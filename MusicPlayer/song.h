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
        const std::string& p = "",
        int d = 0)
        : title(t), artist(a), filePath(p), durationInSeconds(d) {
    }

    //  H:MM:SS 
    std::string getFormattedDuration() const {
        int hours = durationInSeconds / 3600;
        int minutes = (durationInSeconds % 3600) / 60;
        int seconds = durationInSeconds % 60;
        char buffer[24];
        std::snprintf(buffer, sizeof(buffer), "%d:%02d:%02d", hours, minutes, seconds);
        return std::string(buffer);
    }
};

#endif // SONG_H