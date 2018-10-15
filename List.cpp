#include "List.hpp"
#include "Node.hpp"
// List constructor
List::List()
{
    // create new tail Node
    tail = new Node(INT_MAX);
    
    // create new head Node and set its 
    // next pointer to tail
    head = new Node(0); 
    head->next = tail;

    // initialize mark to the value of 1. 
    // This will be used later for bit-masking
    mark = 0x1;

    // Initialize the memory pool and the first block.
    memptr = 0;
    mem = (Node**)malloc(MEM_BLOCK_CNT*sizeof(Node));
    mem[0] = (Node*)malloc(MEM_BLOCK_SIZE*sizeof(Node));

    
}

// these next three algorithms are based off of Algorithm 2 
// of the paper:
// Lock-free Transactions without Rollbacks for Linked Data Structures
bool List::isMarked(Node *n) 
{
  return (bool) ((intptr_t)n & mark);
}

Node *List::getUnmarked(Node *n) 
{
  return (Node *)((intptr_t)n & ~mark);
}

Node *List::getMarked(Node *n) 
{
  return (Node *)((intptr_t)n | mark);
}


// find, add, remove, and contains functions are taken from 
// Tim Harris' paper:
// A pragmatic implementation of non-blocking linked-lists

// you are passing a double Node pointer because
// you want to pass the address of the Node you are 
// trying to redirect. 
Node *List::find(uint32_t key, Node** left)
{
    // create a Node *right so that you have the 
    // rightmost position of the list before you 
    // get to a value that is equal to key or 
    // as close to that value, but greater.
    Node *right;

    // create a Node *left_next so that you can 
    // check, when right is assigned, if right is
    // the immediate successor to left. This checks
    // for if there are any marked Nodes intermitently between
    // left and right that need to be "removed"
    Node *left_next;

    search:
    do 
    {
        // i will traverse through the list, starting
        // from head
        Node *i = head;
        Node *i_next = i->next;

        // the while conditional for this loop states:
        // traverse the list for as long as the traversing Node's
        // key is strictly less than the searched key, or we are 
        // traversing over marked Nodes that have been logically
        // removed
        do 
        {
        	// if the next Node in the list being traversed is 
        	// not marked, in other words, logically removed, then
        	// set the passed in left pointer to point to that Node.
            if (!isMarked(i_next)) 
            {
                (*left) = i;
                left_next = i_next;
            }

            // i iterates through the list, but only containing the 
            // unmarked version of its next pointer
            i = getUnmarked(i_next);

            // break if you reach the end of the list
            if(i == tail)
            {
                break;
            }

            // increment i_next to the next Node in the list
            i_next = i->next;

        } while (isMarked(i_next) || i->key < key);

        // set the rightmost node to the last Node that was traversed
        // in the above for loop
        right = i;

        // if the left and right nodes are adjacent to each other, 
        // i.e. there are no logically removed nodes in between 
        // left and right
        if (left_next == right)
        {
            if((right != tail) && isMarked(right->next))
            {
                goto search;
            }

            // return the rightmost node
            else
            {
                return right;
            }
        }

        // if the above case does not hold, i.e. there are logically 
        // removed nodes between left and right, then you have to 
        // try to switch what left->next points to, to the right Node.
        if(__sync_bool_compare_and_swap(&(*left)->next, left_next, right))
        {
            if(right != tail && isMarked(right->next))
                goto search;
            else
                return right;
        }
    } while (true); 
}

// insert a Node, n, with n->key value of key.
// Insert only if the key value does not already exist in the set.
bool List::add(uint32_t key)
{
    Node* n = NULL;
    Node* left;
    Node* right;

    do {
        // find right node that is closest to key value
        // that is also >= key
        right = find(key, &left);
        if (right->key == key) 
        {
            cout << "could not insert Node with key: " << key << " The node already exists in the set." << endl;
            return false;
        }

        if (n == NULL) {
            uint32_t my_memptr = __sync_fetch_and_add(&memptr, 1);
            uint32_t my_memblock = my_memptr/MEM_BLOCK_SIZE;

            if(mem[my_memblock] == NULL) {
                Node* tmpmem = (Node*)malloc(MEM_BLOCK_SIZE*sizeof(Node));
   
                if(!__sync_bool_compare_and_swap(&mem[my_memblock], NULL, tmpmem)) 
                {
                    free(tmpmem);
                }
            }
            n = &mem[my_memblock][my_memptr%MEM_BLOCK_SIZE];
            n->key = key;
        }
        
        // you want n->next pointing to the right node 
        // because that is the insertion point of the list
        n->next = right;

        // change left->next to point to the newly inserted
        // Node, n. 
        if (__sync_bool_compare_and_swap(&(left->next), right, n)) 
        {
            cout << "successfully inserted Node with key: " << key << endl;
            return true;
        }
    } while (true);
}

// remove Node with key value <key> if it exists
// in the list. If not, return false. 
// The Node is only logically removed, as you do a compare
// and swap on the right->next field.
bool List::remove(uint32_t key)
{
    Node *left;
    Node *right;
    Node *right_next;
    
    do 
    {
        // find right node that is closest to key value
        // that is also >= key
        right = find(key, &left);

        if (right->key != key) 
        {
            cout << "could not remove Node with key: " << key << endl;
            return false;
        }
        right_next = right->next;
        if(!isMarked(right_next))
        {
        	// this compare and swap is utilitzed to see 
        	// if another thread has affected the nodes you're 
        	// trying to manipulate. 
        	if(__sync_bool_compare_and_swap(&(right->next), right_next, getMarked(right_next)))
        	{
        		break;
        	}
        }

    } while (true);

    if(!__sync_bool_compare_and_swap(&(left->next), right, right_next))
    {
    	right = find(right->key, &left);
    }
    cout << "successfully removed Node with key: " << key << endl;
    return true;
}

// search the list for a value. If it exists in the list,
// return true. Else, return false.
bool List::contains(uint32_t key)
{
    Node *right, *left;
    right = find(key, &left); 
    // check if rightmost Node is tail Node
    // or keys do not match
    if(right == tail || right->key != key)
    	return false;
    // else, the keys do match. Return true.
    else
    	return true;
}

// recursive function to print a linked list 
void List::Print(Node *n)
{
    if(n == NULL)
    	return;
    printf("Node [%p] Key [%u]\n", n, n->key); 
    Print(n->next);
}

int main(void)
{
    List *list = new List();
    list->add(15);
    list->add(12);
    list->add(19);
    list->add(12);
    list->remove(12);
    list->remove(27);  
    cout << "result of list->contains(19): " << list->contains(19) << endl; 

    list->Print(list->head); 
    return 0;
}