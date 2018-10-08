#ifndef _LIST_HPP_
#define _LIST_HPP_

#include "Node.hpp"
#include <memory>

class List {
public:
    unique_ptr<Node> head = make_unique<Node>();
    unique_ptr<Node> tail = make_unique<Node>();
    bool add(int item);
    bool remove(int item);
    bool contains(int item);
    List();
};

#endif