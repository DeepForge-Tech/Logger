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
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include <fmt/color.h>

namespace Logger
{
    const char debug_label[6] = "DEBUG";
    const char info_label[5] = "INFO";
    const char error_label[6] = "ERROR";
    const char warning_label[8] = "WARNING";
    const char success_label[8] = "SUCCESS";

    class Logging
    {
    public:

        Logging(const char *Path = nullptr, const char *MaxSize = nullptr, const std::string LogServerURL = "")
        {
#if defined(_WIN32)
            SetConsoleOutputCP(CP_UTF8);
#endif
            if (Path != nullptr)
            {
                logPath = std::move(Path);
                if (MaxSize != nullptr)
                {
                    convertSize(MaxSize);
                }
            }
            logServerURL = std::move(LogServerURL);
        }

        void writeLog(const char *type, std::string log_text);

        void sendError(std::string name_program, std::string architecture, std::string channel,
                       std::string os_name, std::string function_name, std::string log_text);

        void printLogWithDateTime(const char *type, std::string log_text);

        void printLogWithoutDatetime(const char *type, std::string log_text);

        void printLog(const char *type, std::string log_text,bool withDateTime = true);

        void setWithDateTime(bool value);

        void addLogToBuffer(const std::string &log_text);

        void processLogBuffer();

        void processFormattedLogBuffer();

        void setFinished(bool value);

        void notifyBuffer();

    protected:
        std::string logPath;
        std::string logServerURL;
        bool withDateTime = true;
        long MAX_SIZE;
        std::unordered_map<std::string, int> LabelSize = {
            {"byte", 1},
            {"kb", 1024},
            {"mb", 1048576},
            {"gb", 1073741824}};

        std::unordered_map<std::string, int> logColor = {
            {debug_label, 0xB187EB},
            {info_label, 0x2AC3DE},
            {error_label, 0xFF3737},
            {warning_label, 0xFFAB70},
            {success_label, 0xC6E2B5}};

        LogClient::Client client;

        void MakeDirectory(std::string dir);

        void convertSize(std::string size);

        static std::string replaceAll(std::string &str, const std::string &from, const std::string &to);

        static std::string getTime();

        static std::string to_lower(std::string sentence);

        static std::string to_upper(std::string sentence);
    private:
    std::queue<std::string> logBuffer;
        std::mutex bufferMutex;
        std::condition_variable bufferCv;
        std::atomic<bool> finished{false};
    };
}

#endif // LOGGER_HEADER