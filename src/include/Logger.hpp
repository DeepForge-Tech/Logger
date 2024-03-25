#ifndef LOGGER_H
#define LOGGER_H

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <ctime>
#include <exception>
#include <filesystem>
#include <cmath>
#include <concepts>
#include <map>
#include <json/json.h>
#include <Client/LogClient.hpp>

using namespace std;

namespace Logger {
    class Logging {
    public:
        long MAX_SIZE;
        string LogPath;
        map<string, int> LabelSize = {
                {"byte", 1},
                {"kb",   1024},
                {"mb",   1048576},
                {"gb",   1073741824}};
        LogClient::Client client;

        Logging(const char *PathFile = nullptr, const char *MaxSize = nullptr) {
#if defined(_WIN32)
            // Set console code page to UTF-8 so console known how to interpret string data
            SetConsoleOutputCP(CP_UTF8);
#endif
            if (PathFile != nullptr) {
                LogPath = PathFile;
                if (MaxSize != nullptr) {
                    convertSize(MaxSize);
                }
            }
        }

        void SendError(const string& NameProgram,const string& Architecture,const string& Channel,const string& OS_NAME,const string& FunctionName,const string& LogText);

    private:

        /* The 'MakeDirectory' function is used to create a directory (folder) in the file system.*/
        void MakeDirectory(string dir);

        /* The `WriteFile` function is responsible for writing the `value` string to a file specified by the `filename` parameter. */
        void WriteFile(const string& filename, const string& value)
        {
            filesystem::path dir(filename);
            long file_size;
            MakeDirectory(dir.parent_path().string());
            /* The line `fstream file(filename, ios::in | ios::binary | ios::out);` is creating a file stream object named `file` and opening a file specified by the `filename` parameter. The file is opened in binary mode (`ios::binary`) and both input and output operations are allowed (`ios::in | ios::out`). This means that the file can be read from and written to. */
            fstream file(filename, ios::in | ios::binary | ios::out);
            /* The line `file.seekg(0, ios::end);` is used to set the get position indicator of the file stream `file` to the end of the file. This allows us to determine the current size of the file by calling `file.tellg()`, which returns the position of the get pointer. In this case, it is used to check the current size of the file before deciding whether to overwrite the file or append to it. */
            file.seekg(0, ios::end);
            /* The line `long file_size = file.tellg();` is used to determine the current size of the file. */
            file_size = file.tellg();
            if (file_size > MAX_SIZE) {
                file.close();
                /* The line `ofstream new_file(filename, ios::out | ios::binary | ios::trunc);` is creating a new output file stream object named `new_file` and opening a file specified by the `filename` parameter. The file is opened in output mode (`ios::out`), binary mode (`ios::binary`), and truncation mode (`ios::trunc`). */
                ofstream new_file(filename, ios::out | ios::binary | ios::trunc);
                new_file.write(value.c_str(), sizeof(string));
                new_file.close();
            } else {
                file.write(value.c_str(), sizeof(string));
                file.close();
            }
        }

        /* The `convertSize` function is used to convert a given size string into bytes. */
        void convertSize(string size) {
            size = to_lower(size);
            // Converting to bytes
            for (const auto &element: LabelSize) {
                if (size.find(element.first) != string::npos) {
                    size = replaceAll(size, element.first, "");
                    /* The line `MAX_SIZE = stol(size) * element.second;` is calculating the maximum size in bytes based on the given size string and the corresponding label size. */
                    MAX_SIZE = stol(size) * element.second;
                    break;
                }
            }
        }

        static string replaceAll(string str, const string &from, const string &to) {
            size_t start_pos = 0;
            while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
                str.replace(start_pos, from.length(), to);
                start_pos += to.length();
            }
            return str;
        }

        /* The `GetTime()` function is used to get the current time and format it as a string. */
        static string GetTime() {
            /* The line `time_t now = time(0);` is used to get the current time in seconds since the Unix epoch (January 1, 1970). The `time(0)` function returns the current time as a `time_t` object, which is then assigned to the variable `now`. */
            time_t now = time(nullptr);
            struct tm tstruct;
            char buf[80];
            /* The line `tstruct = *localtime(&now);` is assigning the value of the `tm` structure returned by the `localtime` function to the variable `tstruct`. */
            tstruct = *localtime(&now);
            /* The `strftime` function is used to format the time and date values into a string. In this case, `strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct)` is formatting the current time and date into the string `buf` using the specified format. */
            strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
            return buf;
        }

        // Function of make string to lower
        static string to_lower(string sentence) {
            string new_sentence;
            /* The code snippet `for (int i = 0; i < sentence.length(); i++)` is a for loop that iterates over each character in the string `sentence`. */
            for (int i = 0; i < sentence.length(); i++) {
                char ch = sentence[i];
                ch = tolower(ch);
                new_sentence += ch;
            }
            return new_sentence;
        }


    };
}
#endif // LOGGER_H