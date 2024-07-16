#include <Logger.hpp>

std::string Logger::Logging::to_lower(const std::string sentence)
{
    std::string new_sentence;
    for (char ch : sentence)
    {
        ch = tolower(ch);
        new_sentence += ch;
    }
    return new_sentence;
}

std::string Logger::Logging::replaceAll(std::string &str, const std::string &from, const std::string &to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

void Logger::Logging::convertSize(std::string size)
{
    size = to_lower(size);
    for (const auto &element : LabelSize)
    {
        if (size.find(element.first) != std::string::npos)
        {
            size = replaceAll(size, element.first, "");
            MAX_SIZE = stol(size) * element.second;
            break;
        }
    }
}

void Logger::Logging::MakeDirectory(std::string dir)
{
    try
    {
        std::string currentDir;
        std::string fullPath;
        std::string delimiter = "\\";
        size_t pos = 0;
#if defined(_WIN32)
        while ((pos = dir.find(delimiter)) != std::string::npos)
        {
            currentDir = dir.substr(0, pos);
            if (!fullPath.empty())
            {
                fullPath = fullPath + "\\" + currentDir;
                if (!std::filesystem::exists(fullPath))
                {
                    CreateDirectoryA(fullPath.c_str(), nullptr);
                }
            }
            else
            {
                fullPath = currentDir + "\\";
            }
            dir.erase(0, pos + delimiter.length());
        }
        if (!fullPath.empty())
        {
            fullPath = fullPath + "\\" + dir;
        }
        else
        {
            fullPath = dir + "\\";
        }
        if (!std::filesystem::exists(fullPath))
        {
            CreateDirectoryA(fullPath.c_str(), nullptr);
        }
#else
        delimiter = "/";
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
    catch (std::exception &error)
    {
        sendError("Logger", "Empty", "Empty", "Error", "Logger.MakeDirectory", error.what());
    }
}

void Logger::Logging::sendError(std::string name_program, std::string architecture, std::string channel,
                                std::string os_name, std::string function_name, std::string log_text)
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
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}

void Logger::Logging::writeLog(const char *type, std::string log_text)
{
    std::string logDir;
    log_text = fmt::format("[{}]::{}:::{}", getTime(), logInformation[to_upper(type)], log_text);
    if (!logPath.empty())
    {
        logDir = std::filesystem::path(logPath).parent_path().generic_string();
        if (!std::filesystem::exists(logDir))
        {
            std::filesystem::create_directory(logDir);
        }

        std::fstream file;
        file.open(logPath, std::ifstream::ate | std::ios_base::app | std::ios_base::binary);
        if (file.is_open())
        {
            const std::streampos file_size = file.tellg();
            if (file_size > MAX_SIZE)
            {
                file.close();
                std::fstream new_file;
                new_file.open(logPath, std::ios::out | std::ios::binary);
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
}

std::string Logger::Logging::to_upper(std::string sentence)
{
    std::string new_sentence = "";
    for (char ch : sentence)
    {
        ch = toupper(ch);
        new_sentence += ch;
    }
    return new_sentence;
}

void Logger::Logging::addLogToBuffer(const std::string &log_text)
{
    {
        std::lock_guard<std::mutex> lock(bufferMutex);
        logBuffer.push(log_text);
    }
    bufferCv.notify_one();
}

void Logger::Logging::processLogBuffer()
{
    logThread = std::thread(&Logger::Logging::readLogBuffer, this);
}

void Logger::Logging::readLogBuffer()
{
    try
    {
        std::unique_lock<std::mutex> lock(bufferMutex);
        while (!finished.load())
        {
            bufferCv.wait(lock, [this]
                          { return !logBuffer.empty() || finished.load(); });
            while (!logBuffer.empty())
            {
                std::string log = std::move(logBuffer.front());
                logBuffer.pop();
                lock.unlock();
                std::cout << log << std::endl; // Или другая обработка логов
                lock.lock();
            }
        }
    }
    catch (const std::exception &e)
    {
        sendError("Logger", "N/A", "N/A", "Error", "Logger.readLogBuffer", e.what());
    }
    catch (...)
    {
        sendError("Logger", "N/A", "N/A", "Error", "Logger.readLogBuffer", "Unknown exception");
    }
}

void Logger::Logging::setFinished(bool value)
{
    finished = value;
}

void Logger::Logging::notifyBuffer()
{
    bufferCv.notify_one();
}