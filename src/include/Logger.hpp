#ifndef LOGGER_HEADER
#define LOGGER_HEADER

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
#include <LogClient.hpp>

using namespace std;

namespace Logger {
    class Logging {
    public:
        long MAX_SIZE;
        string logPath;
        map<string, int> LabelSize = {
                {"byte", 1},
                {"kb",   1024},
                {"mb",   1048576},
                {"gb",   1073741824}};
        map<string, string> logInformation = {
                {"Debug", "[DEBUG]"},
                {"Info", "[INFO]"},
                {"Error", "[ERROR]"},
                {"Warning", "[WARNING]"},
                {"Success", "[SUCCESS]"}
        };
        LogClient::Client client;

        Logging(const char *path = nullptr, const char *MaxSize = nullptr) {
#if defined(_WIN32)
            // Set console code page to UTF-8 so console known how to interpret string data
            SetConsoleOutputCP(CP_UTF8);
#endif
            if (path != nullptr) {
                logPath = path;
                if (MaxSize != nullptr) {
                    convertSize(MaxSize);
                }
            }
        }
        void writeLog(const char *type, basic_string<char, char_traits<char>, allocator<char>> log_text);
        void sendError(const string& NameProgram,const string& Architecture,const string& Channel,const string& OS_NAME,const string& FunctionName,const string& LogText);

    private:
        /* The 'MakeDirectory' function is used to create a directory (folder) in the file system.*/
        void MakeDirectory(string dir);
        /* The `convertSize` function is used to convert a given size string into bytes. */
        void convertSize(string size);

        static string replaceAll(string str, const string &from, const string &to);
        /* The `GetTime()` function is used to get the current time and format it as a string. */
        static string getTime();
        // Function of make string to lower
        static string to_lower(string sentence);
    };
}
#endif