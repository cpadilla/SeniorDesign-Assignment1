#ifndef _NODE_HPP_
#define _NODE_HPP_

#include <mutex>
#include <memory>

using namespace std;

class Node
{
public:
    int item;
    int key; // Key is just the item value
    bool marked = false;
    unique_ptr<Node> next;
    mutex mtx;
    Node *next;
    void lock();
    void unlock();
    Node();
    Node(int item);
};

#endif