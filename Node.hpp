#ifndef _NODE_HPP_
#define _NODE_HPP_

template <typename T>
class Node
{
    T item;
    int key;
    // TODO: Use Boost smart pointers
    Node * next;
};

#endif