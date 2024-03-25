#include "include/Logger.hpp"

void Logger::Logging::MakeDirectory(string dir) {
    try {
        string currentDir;
        string fullPath;
        string delimiter = "\\";
        size_t pos = 0;
#if defined(_WIN32)
        while ((pos = dir.find(delimiter)) != string::npos) {
            currentDir = dir.substr(0, pos);
            if (!fullPath.empty()) {
                fullPath = fullPath + "\\" + currentDir;
                if (!filesystem::exists(fullPath)) {
                    CreateDirectoryA(fullPath.c_str(), nullptr);
                }
            } else {
                fullPath = currentDir + "\\";
            }
            dir.erase(0, pos + delimiter.length());
        }
        if (!fullPath.empty()) {
            fullPath = fullPath + "\\" + dir;
        } else {
            fullPath = dir + "\\";
        }
        if (!filesystem::exists(fullPath)) {
            CreateDirectoryA(fullPath.c_str(), nullptr);
        }
#else
        while ((pos = dir.find(delimiter)) != string::npos)
        {
            currentDir = dir.substr(0, pos);
            if (!fullPath.empty())
            {
                fullPath = fullPath + "/" + currentDir;
                if (!filesystem::exists(fullPath))
                {
                    filesystem::create_directory(fullPath);
                }
            }
            else
            {
                fullPath = "/" + currentDir;
            }
            dir.erase(0, pos + delimiter.length());
        }
        fullPath = fullPath + "/" + dir;
        if (!filesystem::exists(fullPath))
        {
            filesystem::create_directory(fullPath);
        }
#endif
    }
    catch (exception &error) {
        SendError("Logger", "Empty", "Empty", "Error", "Logger.MakeDirectory", error.what());
    }
}


void Logger::Logging::SendError(const string &NameProgram, const string &Architecture, const string &Channel,
                                       const string &OS_NAME, const string &FunctionName, const string &LogText) {
    Json::Value data;
    data["name_program"] = NameProgram;
    data["architecture"] = Architecture;
    data["channel"] = Channel;
    data["os_name"] = OS_NAME;
    data["function_name"] = FunctionName;
    data["log_text"] = LogText;
    client.POST("https://logserver.alwaysdata.net/api/logs/add_log", data);
}