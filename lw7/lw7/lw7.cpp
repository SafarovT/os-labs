#include <iostream>
#include <filesystem>
#include <string>

namespace
{
    using namespace std;

    filesystem::path ParseArgs(int argc, char* argv[])
    {
        return argc > 1
            ? argv[1]
            : filesystem::current_path();
    }

    // выводить префикс, когда в папке есть подпапки
    void PrintTree(const filesystem::path& dirPath, const std::string& prefix = "")
    {
        try
        {
            if (!filesystem::exists(dirPath) || !filesystem::is_directory(dirPath))
            {
                cout << "Error: path " << dirPath.string() << " does not exist or is not directory" << endl;
                return;
            }

            cout << dirPath.filename().string() << endl;

            vector<filesystem::directory_entry> files;
            vector<filesystem::directory_entry> directories;

            for (const auto& entry : filesystem::directory_iterator(dirPath))
            {
                if (entry.is_directory())
                {
                    directories.push_back(entry);
                }
                else
                {
                    files.push_back(entry);
                }
            }

            bool hasSubdirectories = directories.size();

            for (size_t i = 0; i < files.size(); i++)
            {
                const auto& entry = files[i];
                bool isLast = (i == files.size() - 1 && directories.empty());

                cout
                    << prefix
                    << (hasSubdirectories ? "|   " : "    ")
                    << "[F] "
                    << entry.path().filename().string() << endl;
            }

            for (size_t i = 0; i < directories.size(); i++)
            {
                const auto& entry = directories[i];
                bool isLast = (i == directories.size() - 1);

                cout
                    << prefix
                    << (isLast ? "\\---" : "+---")
                    << "[D] ";
                PrintTree(entry.path(), prefix + (isLast ? "    " : "|   "));
            }
        }
        catch (const filesystem::filesystem_error& e)
        {
            cout << "Runtime error: " << e.what() << endl;
        }
    }
}

int main(int argc, char* argv[])
{
    filesystem::path dirPath = ParseArgs(argc, argv);

    PrintTree(dirPath);

    return EXIT_SUCCESS;
}
