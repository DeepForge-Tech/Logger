#ifndef LOG_CLIENT
#define LOG_CLIENT
#include <iostream>
#include <string>
#include "curl/curl.h"
#include "json/json.h"
#include <map>
#include <exception>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <stdio.h>
#include <chrono>
#include <ctime>
#define LOGSERVER_URL "https://enthusiastic-crow-kilt.cyclic.app/"

using namespace std;
//using namespace Json;

namespace LogClient
{
    size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

    class Client {
    public:
        CURL *curl;
        CURLcode res;
        // string type
        string readBuffer;
        string GET_URL = "https://enthusiastic-crow-kilt.cyclic.app/";
//        Value data;
        filesystem::path currentDir = filesystem::current_path();
        string logsDir = currentDir.generic_string() + "/logs";
        string logPath = logsDir + "/log.txt";
        map<string, string> logInformation = {
                {"Debug", "[DEBUG]"},
                {"Info", "[INFO]"},
                {"Error", "[ERROR]"},
                {"Warning", "[WARNING]"},
                {"Success", "[SUCCESS]"}
        };

        int POST(const char *url,const Json::Value& data);

        int GET(const char *url);

        int writeLog(basic_string<char, char_traits<char>, allocator<char>> log_text, const char *type);

        string getTime();

        Client() {
            curl = curl_easy_init();
        }

    private:
    };
    // Client client;
}
#endif
