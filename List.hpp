#ifndef LLIST_H_ 
#define LLIST_H_

#include <cstdint>
#include <iostream>
#include <cstdlib>
#include <limits.h>
#include <cstdio>
using namespace std;

// boolean CAS_PTR for ease of coding.
#define CAS_PTR_BOOL(addr, old, new) (old == CAS_PTR(addr, old, new))
#define MEM_BLOCK_SIZE 1000000 //16MB (node_t = 16b)
#define MEM_BLOCK_CNT 500 // 8GB of mem max

class Node 
{
public:
    uint32_t key;
    Node* next;

    Node(uint32_t i)
    {
    	key = i;
    	next = nullptr;
    }
};

class List
{
public:
    

    Node *head;
    Node *tail;
    int mark;

    List();
    ~List();
    
    bool isMarked(Node *i);
    Node *getUnmarked(Node* w);
    Node *getMarked(Node* w); 

    Node *find(); 

    bool add(uint32_t key);
    bool remove(uint32_t key);
    bool contains(uint32_t key);

    

    int Size();

    void Print();

private:
    Node* find(uint32_t key, Node** left_node);

private:

    // Memory pool
    Node** mem; // Memory blocks
    uint32_t memptr; // Current cell
};

#endif