#ifndef _NODE_HPP_
#define _NODE_HPP_

#include <mutex>
#include <memory>

using namespace std;

template <typename T>
class AtomicReference
{
private:
    T ref; 
    bool initialMark; 

public:
    bool compareAndSet(T expectedReference, T newReference, bool expectedMark, bool newMark);
    bool attemptMark(T expectedReference, bool newMark);
    Node get(bool marked[]);
}; 

class Node
{
public:
    int item;
    int key; // Key is just the item value
    mutex mtx;
    AtomicReference<Node> *next;
    void lock();
    void unlock();
    Node();
    Node(int item);
};

#endif