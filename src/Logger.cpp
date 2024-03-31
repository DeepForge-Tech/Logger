#include <Logger.hpp>

std::string Logger::Logging::to_lower(const std::string& sentence) {
    std::string new_sentence;
    /* The code snippet `for (int i = 0; i < sentence.length(); i++)` is a for loop that iterates over each character in the string `sentence`. */
    for (int i = 0; i < sentence.length(); i++) {
        char ch = sentence[i];
        ch = tolower(ch);
        new_sentence += ch;
    }
    return new_sentence;
}

std::string Logger::Logging::replaceAll(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

void Logger::Logging::convertSize(std::string size) {
    size = to_lower(size);
    // Converting to bytes
    for (const auto &element: LabelSize) {
        if (size.find(element.first) != std::string::npos) {
            size = replaceAll(size, element.first, "");
            /* The line `MAX_SIZE = stol(size) * element.second;` is calculating the maximum size in bytes based on the given size string and the corresponding label size. */
            MAX_SIZE = stol(size) * element.second;
            break;
        }
    }
}

void Logger::Logging::MakeDirectory(std::string dir) {
    try {
        std::string currentDir;
        std::string fullPath;
        std::string delimiter = "\\";
        size_t pos = 0;
#if defined(_WIN32)
        while ((pos = dir.find(delimiter)) != std::string::npos) {
            currentDir = dir.substr(0, pos);
            if (!fullPath.empty()) {
                fullPath = fullPath + "\\" + currentDir;
                if (!std::filesystem::exists(fullPath)) {
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
        if (!std::filesystem::exists(fullPath)) {
            CreateDirectoryA(fullPath.c_str(), nullptr);
        }
#else
        while ((pos = dir.find(delimiter)) != std::string::npos)
        {
            currentDir = dir.substr(0, pos);
            if (!fullPath.empty())
            {
                fullPath = fullPath + "/" + currentDir;
                if (!std::filesystem::exists(fullPath))
                {
                    std::filesystem::create_directory(fullPath);
                }
            }
            else
            {
                fullPath = "/" + currentDir;
            }
            dir.erase(0, pos + delimiter.length());
        }
        fullPath = fullPath + "/" + dir;
        if (!std::filesystem::exists(fullPath))
        {
            std::filesystem::create_directory(fullPath);
        }
#endif
    }
    catch (std::exception &error) {
        sendError("Logger", "Empty", "Empty", "Error", "Logger.MakeDirectory", error.what());
    }
}


void Logger::Logging::sendError(std::basic_string<char, std::char_traits<char>, std::allocator<char>> name_program,
                                std::basic_string<char, std::char_traits<char>, std::allocator<char>> architecture,
                                std::basic_string<char, std::char_traits<char>, std::allocator<char>> channel,
                                std::basic_string<char, std::char_traits<char>, std::allocator<char>> os_name,
                                std::basic_string<char, std::char_traits<char>, std::allocator<char>> function_name,
                                std::basic_string<char, std::char_traits<char>, std::allocator<char>> log_text)
{
    Json::Value data;
    data["name_program"] = name_program;
    data["architecture"] = architecture;
    data["channel"] = channel;
    data["os_name"] = os_name;
    data["function_name"] = function_name;
    data["log_text"] = log_text;
    client.POST("https://logserver.alwaysdata.net/api/logs/add_log", data);
}

std::string Logger::Logging::getTime()
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

void Logger::Logging::writeLog(const char* type, basic_string<char, char_traits<char>, allocator<char>> log_text)
{
    log_text = "[" + getTime() + "]::" + logInformation[to_lower(type)] + ":::" + log_text;
    std::string logDir = std::filesystem::path(logPath).parent_path().generic_string();
    if (!std::filesystem::exists(logDir)) {
        std::filesystem::create_directory(logDir);
    }

    std::fstream file;
    file.open(logPath,std::ifstream::ate | std::ios_base::app | std::ios_base::binary);
    if (file.is_open()) {
        // Get the current size of the file
        const std::streampos file_size = file.tellg();
        if (file_size > MAX_SIZE) {
            file.close();
            std::fstream new_file;
            new_file.open(logPath,std::ios::out | std::ios::binary);
            new_file << log_text << std::endl;
            new_file.close();
        }
        else
        {
            file << log_text << std::endl;
            file.close();
        }
    }
}