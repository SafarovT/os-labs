#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <cctype>
#include <string>
#include <stop_token>
#include <stdexcept>
#include <mutex>
#include "../../../../auto/lw1/Transition/FileUtils.h"

using namespace std;

// принимать и не нужно преобр.
char FlipCase(unsigned char ch) {
    if (std::islower(ch))
    {
        return std::toupper(ch);
    }
    else if (std::isupper(ch))
    {
        return std::tolower(ch);
    }
    return ch;
}

bool CheckArgsCount(int argc)
{
    return argc >= 2;
}

void ProcessFile(const string& inputFileName, const string& outputFileName)
{
    try
    {
        ifstream inputFile;
        ofstream outputFile;
        OpenFile(inputFile, inputFileName);
        OpenFile(outputFile, outputFileName);

        char ch;
        while (inputFile.get(ch))
        {
            char flippedCh = FlipCase(ch);
            outputFile.put(flippedCh);
        }

        CheckFileRuntime(inputFile);
        CheckFileRuntime(outputFile);

    }
    catch (const exception& e)
    {
        cout << "Failed to transform file " << inputFileName << ": " << e.what() << endl;
    }
}

// проверить если код внутри потока сделает join самому себе - неопределенное поведение
// выяснить что произойдет если вызвался деструктор в неджойнабл треде - исключение, тк. стандарт требует чтобы все завершились
// выяснить что произойдет если ф-ия выбросит исключение - программа завершится, то поток завершится а программа завершится с ошибкой

int main(int argc, char* argv[])
{
    if (!CheckArgsCount(argc))
    {
        cout << "Usage: " << argv[0] << " <file1.txt> ... <fileN.txt>" << endl;
        return EXIT_FAILURE;
    }
    
    // jthread
    vector<jthread> threads;

    for (int i = 1; i < argc; ++i)
    {
        string input_filename = argv[i];
        string output_filename = input_filename + ".out";

        // во что надо оборачивать
        threads.emplace_back(jthread(ProcessFile, input_filename, output_filename));
    }

    for (auto& thread : threads)
    {
        // в какой момент тред джойнабл возвращает false - empty, detach, join
        if (thread.joinable())
        {
            thread.join();
        }
    }

    return EXIT_SUCCESS;
}
