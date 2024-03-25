#include <Logger.hpp>

string Logger::Logging::to_lower(string sentence) {
    string new_sentence;
    /* The code snippet `for (int i = 0; i < sentence.length(); i++)` is a for loop that iterates over each character in the string `sentence`. */
    for (int i = 0; i < sentence.length(); i++) {
        char ch = sentence[i];
        ch = tolower(ch);
        new_sentence += ch;
    }
    return new_sentence;
}

string Logger::Logging::replaceAll(string str, const string &from, const string &to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

void Logger::Logging::convertSize(string size) {
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
        sendError("Logger", "Empty", "Empty", "Error", "Logger.MakeDirectory", error.what());
    }
}


void Logger::Logging::sendError(const string &NameProgram, const string &Architecture, const string &Channel,
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

string Logger::Logging::getTime()
{
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}

void Logger::Logging::writeLog(const char *type,basic_string<char, char_traits<char>, allocator<char>> log_text)
{
    log_text = "[" + getTime() + "]::" + logInformation[to_lower(type)] + ":::" + log_text;
    filesystem::path dir(logPath);
    long file_size;
    MakeDirectory(dir.parent_path().string());
    /* The line `fstream file(path, ios::in | ios::binary | ios::out);` is creating a file stream object named `file` and opening a file specified by the `path` parameter. The file is opened in binary mode (`ios::binary`) and both input and output operations are allowed (`ios::in | ios::out`). This means that the file can be read from and written to. */
    fstream file(logPath, ios::in | ios::binary | ios::out);
    /* The line `file.seekg(0, ios::end);` is used to set the get position indicator of the file stream `file` to the end of the file. This allows us to determine the current size of the file by calling `file.tellg()`, which returns the position of the get pointer. In this case, it is used to check the current size of the file before deciding whether to overwrite the file or append to it. */
    file.seekg(0, ios::end);
    /* The line `long file_size = file.tellg();` is used to determine the current size of the file. */
    file_size = file.tellg();
    if (file_size > MAX_SIZE) {
        file.close();
        /* The line `ofstream new_file(path, ios::out | ios::binary | ios::trunc);` is creating a new output file stream object named `new_file` and opening a file specified by the `path` parameter. The file is opened in output mode (`ios::out`), binary mode (`ios::binary`), and truncation mode (`ios::trunc`). */
        ofstream new_file(logPath, ios::out | ios::binary | ios::trunc);
        new_file << log_text << endl;
        new_file.close();
    } else {
        file << log_text << endl;
        file.close();
    }
}