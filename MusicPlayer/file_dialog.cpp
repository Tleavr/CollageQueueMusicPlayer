#include "file_dialog.h"

// This file does not include raylib, so windows.h can be used
// without any name collisions (Rectangle, CloseWindow, ...).
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <commdlg.h>
#endif

std::string OpenMusicFileDialog()
{
#ifdef _WIN32
    char filename[MAX_PATH] = {};
    OPENFILENAMEA dialog = {};
    dialog.lStructSize = sizeof(dialog);
    dialog.lpstrFilter =
        "Audio Files (*.mp3;*.wav;*.ogg)\0*.mp3;*.wav;*.ogg\0"
        "All Files (*.*)\0*.*\0";
    dialog.lpstrFile = filename;
    dialog.nMaxFile = MAX_PATH;
    dialog.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    if (GetOpenFileNameA(&dialog)) return filename;
#endif
    return {};
}