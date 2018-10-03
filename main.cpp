/**
 * Assignment 1 - Concurrent Linked List
 */
#include "main.hpp"

using namespace std;

void listModifier(int ID)
{
    int iterations = 0;
    while (iterations < 100)
    {
        auto now_c = chrono::system_clock::to_time_t(chrono::system_clock::now() - std::chrono::hours(24));
        auto time = localtime(&now_c);
        // TODO: Format string to look better, output milliseconds as well
        string out = to_string(time->tm_hour) + ":" + to_string(time->tm_min) + ":" + to_string(time->tm_sec) +
                    " Thread " + to_string(ID) + ": modifying stack.\n";

        cout << out;

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
}