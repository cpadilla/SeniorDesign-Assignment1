#include "Node.hpp"

Node::Node()
{

};

Node::Node(int item) : item(item), key(item)
{

};

void Node::lock()
{
    mtx.lock();
};

void Node::unlock()
{
    mtx.unlock();
};