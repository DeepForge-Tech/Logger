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
#include <unordered_map>
#include <json/json.h>
#include <LogClient/LogClient.hpp>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#define FMT_HEADER_ONLY
#include <fmt/format.h>

namespace Logger
{
    class Logging
    {
    public:
        long MAX_SIZE;
        std::string logPath;
        std::unordered_map<std::string, int> LabelSize = {
            {"byte", 1},
            {"kb", 1024},
            {"mb", 1048576},
            {"gb", 1073741824}};
        std::unordered_map<std::string, std::string> logInformation = {
            {"DEBUG", "[DEBUG]"},
            {"INFO", "[INFO]"},
            {"ERROR", "[ERROR]"},
            {"WARNING", "[WARNING]"},
            {"SUCCESS", "[SUCCESS]"}};
        LogClient::Client client;

        Logging(const char *path = nullptr, const char *MaxSize = nullptr)
        {
#if defined(_WIN32)
            // Set console code page to UTF-8 so console known how to interpret string data
            SetConsoleOutputCP(CP_UTF8);
#endif
            if (path != nullptr)
            {
                logPath = path;
                if (MaxSize != nullptr)
                {
                    convertSize(MaxSize);
                }
            }
        }

        void writeLog(const char *type, std::string log_text);
        void sendError(std::string name_program,
                       std::string architecture,
                       std::string channel,
                       std::string os_name,
                       std::string function_name,
                       std::string log_text);

        void addLogToBuffer(const std::string &log_text);
        void processLogBuffer();
        void setFinished(bool value);

        void notifyBuffer();

    protected:
        void MakeDirectory(std::string dir);
        void convertSize(std::string size);
        static std::string replaceAll(std::string &str, const std::string &from, const std::string &to);
        static std::string getTime();
        static std::string to_lower(std::string sentence);
        static std::string to_upper(std::string sentence);

    private:
        std::vector<std::string> logBuffer;
        std::mutex bufferMutex;
        std::condition_variable bufferCv;
        std::atomic<bool> finished{false};
    };
}

#endif // LOGGER_HEADER
