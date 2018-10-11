#ifndef _WINDOW_HPP_
#define _WINDOW_HPP_

#include "Node.hpp"

using namespace std;

class Window
{
public:
	Node pred, curr;

	Window(Node myPred, Node&myCurr); 

	Window find(Node& head, int key); 

}