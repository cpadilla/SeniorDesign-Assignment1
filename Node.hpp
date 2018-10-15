#ifndef _NODE_HPP_
#define _NODE_HPP_

class Node 
{
public:
    uint32_t key;
    Node* next;

    Node(uint32_t i); 
};

#endif