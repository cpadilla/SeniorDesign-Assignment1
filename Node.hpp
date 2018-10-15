#ifndef _NODE_HPP_
#define _NODE_HPP_
#include <cstdint>

class Node 
{
public:
    uint32_t key;
    Node* next;

    Node(uint32_t i); 
};

#endif