#include "List.hpp"

List::List()
{
    head->key = -1;
    tail->key = 2147483647;
    head->next = move(tail);
}

bool List::add(int item)
{
    return true;
}

bool List::remove(int item)
{
    return true;
}

bool List::contains(int item)
{
    return true;
}
