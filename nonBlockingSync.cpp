#include <iostream>
#include <climits>
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

	LockFreeList()
	{
		head = new Node(0);
		tail = new Node(INT_MAX); 
		head->next = tail;
		mark = 0x1;
	}

	bool isMarked(Node *n)
	{
		return (bool)((intptr_t)n & mark); 
	}

	Node *getUnmarked(Node *n)
	{
		return (Node *)((intptr_t)n & mark);
	}

	Node *getMarked(Node *n)
	{
		return (Node *)((intptr_t)n | mark);
	}

	Node *find(uint32_t key, Node **left)
	{
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
					(*left) = i;
					left_next - i_next;
				}

				i = get_unmarked_ref(i_next);

				if(i == tail)
				{
					break;
				}

				i_next = i->next;
			} while (isMarked(i_next) || i->key < key);

			right = i;

			if(left_next == right)
			{
				if((right != tail) && isMarked(right->next))
					goto repeat_search;
				else
					return right;
			}

			if(__sync_bool_compare_and_swap(&(*left)->next, left_next, right))
			{
				if((right != tail) && isMarked(right->next))
					goto repeat_search;
				else
					return right;
			}
		} while (true);
	}

	bool search(uint32_t key)
	{
		Node *left;
		return (find(key, &left)->key == key);
	}

	bool insert(uint32_t key)
	{
		Node *n = nullptr;
		Node *left;
		Node *right;

		do
		{
			right = find(key, &left)
			if(right->key == key)
			{
				return false;
			}

			if(n == nullptr)
			{
				uint32_t my_memptr = __sync_fetch_and_add(&memptr, 1);
				uint32_t my_memblock = my_memptr/MEM_BLOCK_SIZE;

				if(mem[my_memblock] == NULL)
				{
					Node *temp = (*Node)malloc(MEM_BLOCK_SIZE * sizeof(Node)); 

					if(!__sync_bool_compare_and_swap(&mem[my_memblock], NULL, temp))
					{
						free(temp);
					}
				}

				n = &mem[my_memblock][my_memptr%MEM_BLOCK_SIZE];
				n->key = key;
			}

			n->next = right;

			if(__sync_bool_compare_and_swap(&(left->next), right, n))
				return true;
		} while (true); 
	}
};


int main(void)
{
	LockFreeList *list = new LockFreeList();
	cout << "this is &list->head: " << &list->head << endl;
	cout << "this is list->head->key: " << list->head->key << endl;

	return 0;
}
