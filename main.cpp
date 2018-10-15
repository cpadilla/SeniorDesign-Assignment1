/**
 * Assignment 1 - Concurrent Linked List
 */
#include "main.hpp"

using namespace std;

List * linkedList = new List();

void listModifier(int ID)
{
    int iterations = 0;

    while (iterations < 100)
    {
        // TODO: Have chance to do random operation (add/remove/contains)
        // currently just adds
        int randomChoice = rand() % 3;
        int randomIncrement = rand() % 2;

        auto now_c = chrono::system_clock::to_time_t(chrono::system_clock::now() - std::chrono::hours(24));
        auto time = localtime(&now_c);
        // TODO: Format string to look better, output milliseconds as well
        int val = iterations + 100 * ID;
        string out = "";
        int removed = 0;
        int contains = 0;
        switch (randomChoice) {
            // add
            case 0:
                out = to_string(time->tm_hour) + ":" + to_string(time->tm_min) + ":" + to_string(time->tm_sec) +
                            " Thread " + to_string(ID) + ": adding " + to_string(val) +" to the stack.\n";

                cout << out;

                linkedList->add(val);

                // randomly decide to increment the next number we oprate with
                if (randomIncrement == 0) {
                    iterations++;
                }

                break;
            // remove
            case 1:
                out = to_string(time->tm_hour) + ":" + to_string(time->tm_min) + ":" + to_string(time->tm_sec) + " Thread " + to_string(ID) + ": removing " + to_string(val) +" from the stack.\n"; cout << out; 
                removed = linkedList->remove(val);
                break;
            // contains
            case 2:
                contains = linkedList->contains(val);
                out = to_string(time->tm_hour) + ":" + to_string(time->tm_min) + ":" + to_string(time->tm_sec) +
                            " Thread " + to_string(ID) + ": contains(" + to_string(val) +") = "+to_string(contains)+".\n";

                cout << out;

                break;
        }

        this_thread::sleep_for(chrono::milliseconds(rand()%100));
    }
}

int main(int argc, char* argv[])
{

    cout << "Assignment 1 - Concurrent Linked List" << endl;

    thread worker1 (listModifier, 1);
    thread worker2 (listModifier, 2);

    cout << "Threads executing concurrently...\n";

    worker1.join();
    worker2.join();

    cout << "Threads completed tasks.\n";
    

    linkedList->Print(linkedList->head); 
}