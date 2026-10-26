// we include the file header Queue.h to use it's class 
// becuse this file is implementation to fuc i decleard there
#include "Queue.h" //also Queus.h include the file song.h 

#include <iostream>
using namespace std; 
 
// الدالة البناء — تهيئة Queue فارغة
Queue::Queue() { //here i invoke the func Queue() constructure from class Queue in header file Queue.h
    front = nullptr; // there is no songs and no rode at the begainig of the queue
    rear = nullptr; // its like initial value 
    size = 0; //  songs in the queue size 
}
 
// دالة الحذف — تحرير جميع العُقد من الذاكرة
Queue::~Queue() {
    clear(); // desructure  and new fuction inside well be implemented lated
}
 
// إضافة أغنية في نهاية الـ Queue
//no return value | Queue data type class invoke | enqueue is the fuction to add element song invoke
void Queue::enqueue(Song song) { // song from song.h , name, artist , durration
    
    /*
    ينشئ عقدة جديدة في الذاكرة، ويضع عنوانها في المؤشر newNode.

    Node* يعني مؤشرًا إلى عقدة من النوع Node.
    new يحجز مساحة للعقدة في الذاكرة.
    Node مُعرّف داخل Queue.h، ويحتوي على أغنية ومؤشر للعقدة التالية.
    */
    Node* newNode = new Node(); 
    newNode->data = song;
    newNode->next = nullptr;
    
    if (isEmpty()) {
        // الأغنية الأولى
        front = newNode;
        rear = newNode;
    } else {
        // ربط العقدة الجديدة بنهاية الـ Queue
        rear->next = newNode;
        rear = newNode;
    }
    size++;
    cout << "[+] Added: " << song.name << " - " << song.artist << "\n";
}
 
// تشغيل الأغنية الأولى وحذفها من الـ Queue
Song Queue::dequeue() {
    if (isEmpty()) {
        cout << "[ERROR] Queue is empty!\n";
        return {"", "", 0};
    }
    
    Node* temp = front;
    Song song = temp->data;
    front = front->next;
    
    if (front == nullptr) {
        rear = nullptr;
    }
    
    delete temp;
    size--;
    
    cout << "[PLAYING] " << song.name << " - " << song.artist << "\n";
    return song;
}
 
// عرض الأغنية التالية بدون حذفها
Song Queue::peek() {
    if (isEmpty()) {
        cout << "[ERROR] Queue is empty!\n";
        return {"", "", 0};
    }
    
    cout << "[NEXT] " << front->data.name << " - " << front->data.artist << "\n";
    return front->data;
}
 
// عرض جميع الأغاني في الـ Queue
void Queue::display() {
    if (isEmpty()) {
        cout << "[ERROR] Queue is empty!\n";
        return;
    }
    
    cout << "\n--- PLAYLIST ---\n";
    Node* current = front;
    int position = 1;
    
    while (current != nullptr) {
        cout << position << ". " << current->data.name 
             << " - " << current->data.artist 
             << " (" << current->data.duration << "s)\n";
        current = current->next;
        position++;
    }
}
 
// حذف جميع الأغاني من الـ Queue
void Queue::clear() {
    Node* current = front;
    while (current != nullptr) {
        Node* temp = current;
        current = current->next;
        delete temp;
    }
    front = nullptr;
    rear = nullptr;
    size = 0;
    cout << "[CLEARED] Playlist cleared!\n";
}
 
// الحصول على عدد الأغاني
int Queue::getSize() {
    return size;
}
 
// التحقق إذا الـ Queue فارغة
bool Queue::isEmpty() {
    return size == 0;
}
 
// حساب الوقت الكلي للأغاني
int Queue::getTotalDuration() {
    if (isEmpty()) return 0;
    
    int total = 0;
    Node* current = front;
    
    while (current != nullptr) {
        total += current->data.duration;
        current = current->next;
    }
    
    return total;
}