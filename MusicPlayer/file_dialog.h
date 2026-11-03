#ifndef FILE_DIALOG_H
#define FILE_DIALOG_H

#include <string>

// Opens the system "choose a music file" dialog.
// Returns the chosen path, or an empty string if the user cancelled.
std::string OpenMusicFileDialog();

#endif // FILE_DIALOG_H
