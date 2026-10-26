#include <iostream>
#include "Queue.h"
using namespace std;
 
int main() {
    Queue playlist;
    int choice;
    bool running = true;
    
    cout << "===== PLAYLIST MANAGER =====\n\n";
    
    while (running) {
        cout << "\n1. Add Song\n";
        cout << "2. Play Next\n";
        cout << "3. View Next\n";
        cout << "4. Display Playlist\n";
        cout << "5. Clear Playlist\n";
        cout << "6. Show Statistics\n";
        cout << "7. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();
        
        switch (choice) {
            case 1: {
                // إضافة أغنية
                string name, artist;
                int duration;
                
                cout << "Song name: ";
                getline(cin, name);
                
                cout << "Artist name: ";
                getline(cin, artist);
                
                cout << "Duration (seconds): ";
                cin >> duration;
                
                if (duration > 0) {
                    playlist.enqueue({name, artist, duration});
                } else {
                    cout << "[ERROR] Duration must be positive!\n";
                }
                break;
            }
            
            case 2: {
                // تشغيل الأغنية التالية
                if (playlist.isEmpty()) {
                    cout << "[ERROR] Playlist is empty!\n";
                } else {
                    playlist.dequeue();
                }
                break;
            }
            
            case 3: {
                // عرض الأغنية التالية
                if (playlist.isEmpty()) {
                    cout << "[ERROR] Playlist is empty!\n";
                } else {
                    playlist.peek();
                }
                break;
            }
            
            case 4: {
                // عرض جميع الأغاني
                playlist.display();
                break;
            }
            
            case 5: {
                // حذف جميع الأغاني
                if (playlist.isEmpty()) {
                    cout << "[ERROR] Playlist is already empty!\n";
                } else {
                    playlist.clear();
                }
                break;
            }
            
            case 6: {
                // عرض الإحصائيات
                int total = playlist.getTotalDuration();
                int minutes = total / 60;
                int seconds = total % 60;
                
                cout << "\n--- STATISTICS ---\n";
                cout << "Total Songs: " << playlist.getSize() << "\n";
                cout << "Total Duration: " << minutes << "m " << seconds << "s\n";
                break;
            }
            
            case 7: {
                cout << "\nGoodbye!\n";
                running = false;
                break;
            }
            
            default:
                cout << "[ERROR] Invalid choice!\n";
        }
    }
    
    return 0;
}