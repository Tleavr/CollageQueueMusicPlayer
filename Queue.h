#ifndef QUEUE_H
#define QUEUE_H
 
#include "song.h"
 

// new class for the queue
class Queue {
private:
// here i build a new struct 
// make sure it's protected
//to reprsent a node as box
    struct Node {

        //this is one part of our node
        //it means one single song
        //inside one node of our strucure
        Song data;
        //this is the second part of our one single node 
        //wich is a pointer to the next node
        Node* next;
    };
    // pointer to our first song and if we delete the deletion will start from here
    Node* front;
    //pointer to the last song we added last songs usually added to this side
    Node* rear;
    //the number of songs we have ( is it the number of the nodes also ?)
    //it grows when we enqueu songs and reduce when we dequeue
    int size;
    
//public fuction to be used by other files
public:
    // this is the constructure
    // يُستدعى تلقائيًا عند إنشاء طابور جديد
    Queue();


    // this is the destructure
    // يُستدعى عند انتهاء عمر الطابور، وغالبًا يُستخدم لتنظيف الذاكرة التي حُجزت للعقد.
    ~Queue();
    // void it means the fuction don't return a value
    // we putting the structure Song type as argument
    // i'm gonna use this function as tool  to add song to the queue
    void enqueue(Song song);
    // the data type is song the function is dequeue( is it gonna be used for remove a song from the queue?) 
    Song dequeue();
    //this fuction is the repossible for show the song in the front of the queue without removing it
    Song peek();
    // this func will be used to show all the queue content
    void display();
    // this func will be used to clear the queue and free the memo
    void clear();

    // to get the size of the elemts as integer number 
    int getSize();

    //bolean it gonna be as switch to check if the queue is emptiy
    bool isEmpty();

    // this fuc will bw the counter for the total durration of all songs combined
    int getTotalDuration();
    
}; // end of the class
 
#endif