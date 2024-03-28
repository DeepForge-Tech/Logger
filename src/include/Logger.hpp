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
#include <LogClient/LogClient.hpp>

namespace Logger {
    class Logging {
    public:
        long MAX_SIZE;
        std::string logPath;
        std::map<std::string, int> LabelSize = {
                {"byte", 1},
                {"kb",   1024},
                {"mb",   1048576},
                {"gb",   1073741824}};
        std::map<std::string, std::string> logInformation = {
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
        void writeLog(const char *type, std::basic_string<char, std::char_traits<char>, std::allocator<char>> log_text);
        void sendError(std::basic_string<char, std::char_traits<char>, std::allocator<char>> name_program,
                        std::basic_string<char, std::char_traits<char>, std::allocator<char>> architecture,
                        std::basic_string<char, std::char_traits<char>, std::allocator<char>> channel,
                        std::basic_string<char, std::char_traits<char>, std::allocator<char>> os_name,
                        std::basic_string<char, std::char_traits<char>, std::allocator<char>> function_name,
                        std::basic_string<char, std::char_traits<char>, std::allocator<char>> log_text);

    private:
        /* The 'MakeDirectory' function is used to create a directory (folder) in the file system.*/
        void MakeDirectory(std::string dir);
        /* The `convertSize` function is used to convert a given size string into bytes. */
        void convertSize(std::string size);

        static std::string replaceAll(std::string& str, const std::string& from, const std::string& to);
        /* The `GetTime()` function is used to get the current time and format it as a string. */
        static std::string getTime();
        // Function of make string to lower
        static std::string to_lower(const std::string& sentence);
    };
}
#endif
