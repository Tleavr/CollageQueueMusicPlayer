#ifndef SONG_H
#define SONG_H
 
#include <string>
using namespace std;
 
// this is the songs struct 
// where each song have 1. name  2.auther 3.durration

struct Song {
    string name;
    string artist;
    int duration;
};
 
#endif