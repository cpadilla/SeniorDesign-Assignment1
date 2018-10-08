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

        auto now_c = chrono::system_clock::to_time_t(chrono::system_clock::now() - std::chrono::hours(24));
        auto time = localtime(&now_c);
        // TODO: Format string to look better, output milliseconds as well
        int val = iterations + 100 * ID;
        string out = to_string(time->tm_hour) + ":" + to_string(time->tm_min) + ":" + to_string(time->tm_sec) +
                    " Thread " + to_string(ID) + ": adding " + to_string(val) +" to the stack.\n";

        cout << out;

        linkedList->add(val);

        this_thread::sleep_for(chrono::milliseconds(rand()%100));
        iterations++;
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
}