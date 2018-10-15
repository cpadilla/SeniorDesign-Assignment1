#include <iostream>
#include <climits>

#define MEM_BLOCK_SIZE 1000000
using namespace std;

class Node
{
public:
	uint32_t key;
	Node *next;

	Node(uint32_t i)
	{
		key = i;
		next = nullptr;
	}
};

class LockFreeList
{
public:
	Node *head;
	Node *tail;
	int mark;

	Node **mem;
	uint32_t memptr;

	LockFreeList()
	{
		head = new Node(0);
		tail = new Node(INT_MAX); 
		head->next = tail;
		mark = 0x1;
	}

	// checks to see if there has been a logical
	// removal of a node from the list
	bool isMarked(Node *n)
	{
		return (bool)((intptr_t)n & mark); 
	}

	// gets the unmarked reference to a node
	// used for comparisons in Compare And Swap later on
	Node *getUnmarked(Node *n)
	{
		return (Node *)((intptr_t)n & mark);
	}

	// gets the marked reference to a node
	// used for comparisons in Compare and Swap later on
	Node *getMarked(Node *n)
	{
		return (Node *)((intptr_t)n | mark);
	}

	// function to search for the two nodes that are closest to your
	// desired key. 
	Node *find(uint32_t key, Node **left)
	{
		// you want a left_next node so that you
		// can compare it later on. If left_next != right, 
		// then there has been a logical removal of a node somewhere
		// in the list.
		Node *right;
		Node *left_next;

		repeat_search:
		do
		{
			Node *i = head;
			Node *i_next = i->next;

			do
			{

				if(!isMarked(i_next))
				{
					// you're passing to the function an address 
					// to a pointer, called left. By doing this dereference,
					// you're setting that pointer Node to i.
	
					(*left) = i;
					left_next = i_next;
	
				}

				i = getUnmarked(i_next);

				if(i == tail)
				{
					cout << "breaking out of if i == tail" << endl;
					break;
				}

				// this should not be pointing to a nullptr
				// because of the above break statement.

				i_next = i->next;

			} while (isMarked(i_next) || i->key < key);
			right = i;
			// check to see if the nodes are adjacent to 
			// each other, like described above
			if(left_next == right)
			{
				if((right != tail) && isMarked(right->next))
					goto repeat_search;
				else
					return right;
			}

			// if the nodes are not adjacent and you didn't return,
			// attempt to logically remove those intermediary nodes from the list. 
			if(__sync_bool_compare_and_swap(&(*left)->next, left_next, right))
			{
				if((right != tail) && isMarked(right->next))
					goto repeat_search;
				else
					return right;
			}
		} while (true);
	}

	// function to check to see if the node that gets returned 
	// from find() is a node that equals the key that you pass in, 
	// or is a key that is strictly greater than that key
	bool search(uint32_t key)
	{
		Node *n;
		return (find(key, &n)->key == key);
	}

	bool insert(uint32_t key)
	{
		Node *n = nullptr;
		Node *left;
		Node *right;

		do
		{
			// set right node to whatever gets returned from
			// find(). This should be >= the key that we're trying to insert.
			// If  equal, you can't add the value to the Set.
			right = find(key, &left);
			cout << "this is right->key: " << right->key << endl;
			if(right->key == key)
			{
				cout << "unsuccessfully added Node with key: " << key << endl;
				return false;
			}

			if(n == nullptr)
			{
				uint32_t my_memptr = __sync_fetch_and_add(&memptr, 1);
				uint32_t my_memblock = my_memptr/MEM_BLOCK_SIZE;

				if(mem[my_memblock] == NULL)
				{
					Node *temp = (Node *)malloc(MEM_BLOCK_SIZE * sizeof(Node)); 

					if(!__sync_bool_compare_and_swap(&mem[my_memblock], NULL, temp))
					{
						free(temp);
					}
				}

				// initialize n, the node that we're inserting, 
				// into memory.
				n = &mem[my_memblock][my_memptr%MEM_BLOCK_SIZE];
				n->key = key;
			}

			// set n's place in the list to be the predecessor of 
			// right
			n->next = right;

			// you expect that left->next should point to n now, 
			// so attempt to assign it that way. Can't make any assumptions,
			// so do a Compare And Swap evaluation first.
			if(__sync_bool_compare_and_swap(&(left->next), right, n))
			{
				cout << "successfully added Node with key: " << key << endl; 
				return true;
			}

		} while (true); 
	}

	bool remove(uint32_t key)
	{
		Node *left;
		Node *right;

		do
		{
			// search for node that has value closest to key that 
			// is also >= to it.
			right = find(key, &left);

			// if right->key != key, then you know
			// key doesn't exist in the list.
			if(right->key != key)
			{
				cout << "unsuccessfully removed Node with key: " << key << endl;
				return false;
			}

			// this if statement will check to see if it's
			// possible to mark the next field of right, thus 
			// logically removing right from the list. 
			if(__sync_bool_compare_and_swap(&(right->next), getUnmarked(right->next), getUnmarked(right->next)))
			{
				__sync_bool_compare_and_swap(&(left->next), right, getUnmarked(right->next));
				cout << "successfully removed Node with key: " << key << endl; 
				return true;
			}
		} while (true);
	}
};


int main(void)
{
	LockFreeList *list = new LockFreeList();
	cout << "this is &list->head: " << &list->head << endl;
	cout << "this is list->head->key: " << list->head->key << endl;
	cout << "this is list->tail->key: " << list->tail->key << endl;
	cout << "this is list->head->next: " << list->head->next << endl;
	cout << "this is list->tail: " << list->tail << endl;

	list->insert(15);
	list->insert(12);
	list->insert(19);
	list->insert(12);
	list->remove(12);
	list->remove(27); 
	return 0;
}
