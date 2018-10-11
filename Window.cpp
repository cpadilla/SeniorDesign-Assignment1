#include "Window.hpp"

Window::Window Window::find(Node *head, int key)
{
	Node pred = curr = succ = nullptr; 
	bool marked[] = {false};
	bool snip;
retry: 
	while(true)
	{
		pred = head;
		// curr = pred.next figure out how to do the getReference()
		while(true)
		{
			// get is a method that returns a type T, but I 
			succ = curr.next.get(marked);
			while(marked[0])
			{
				// compareAndSet method used for testing expected reference and mark values. 
				// if both succeed, replace with updated reference and marks. 
				snip = pred.next.compareAndSet(curr, succ, false, false); 

				if(!snip)
				{
					goto retry; 
				}
				curr = succ;
				succ = curr.next.get(marked); 
			}

			if(curr.key > key)
			{
				return new Window(pred, curr);
			}

			pred = curr;
			currr = succ; 
		}
	}
}