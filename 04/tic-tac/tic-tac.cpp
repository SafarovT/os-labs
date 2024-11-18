#include <iostream>
#include <thread>
#include <syncstream>
#include <stop_token>
#include "Time.h"

using namespace std;

void TicThread(stop_token stoken)
{
    while (!stoken.stop_requested())
    {
        osyncstream(cout) << CurrentTimeToString() << " Tic" << endl;
        this_thread::sleep_for(chrono::seconds(2));
    }
    osyncstream(cout) << "Tic thread completed" << endl;
}

void TacThread(stop_token stoken)
{
    while (!stoken.stop_requested())
    {
        osyncstream(cout) << CurrentTimeToString() << " Tac" << endl;
        this_thread::sleep_for(chrono::seconds(3));
    }
    osyncstream(cout) << "Tac thread completed" << endl;
}

int main()
{
    jthread ticThread(TicThread);
    jthread tacThread(TacThread);

    string input;
    getline(cin, input);

    return EXIT_SUCCESS;
}