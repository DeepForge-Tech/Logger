#include <Logger/Logger.hpp>

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
        const std::string delimiter = "\\";
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
    if (!logServerURL.empty())
    {
        Json::Value data;
        data["name_program"] = name_program;
        data["architecture"] = architecture;
        data["channel"] = channel;
        data["os_name"] = os_name;
        data["function_name"] = function_name;
        data["log_text"] = log_text;
        client.POST(logServerURL.c_str(), data);
    }
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
    log_text = fmt::format("{} | {} | {}", getTime(), type, log_text);
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

void Logger::Logging::printLogWithDateTime(const char *type, std::string log_text)
{
    log_text = fmt::format(
        "{} | {} | {}",
        getTime(),
        fmt::format(fg(fmt::color(logColor[type])), "{}", type),
        log_text);
    std::cout << log_text << std::endl;
}

void Logger::Logging::printLogWithoutDatetime(const char *type, std::string log_text)
{
    log_text = fmt::format(
        "[{}]::{}",
        fmt::format(fg(fmt::color(logColor[type])), "{}", type),
        log_text);
    std::cout << log_text << std::endl;
}

void Logger::Logging::printLog(const char *type, std::string log_text,bool withDateTime)
{
    if (withDateTime)
    {
        log_text = fmt::format(
            "{} | {} | {}",
            getTime(),
            fmt::format(fg(fmt::color(logColor[type])), "{}", type),
            log_text);
    }
    else
    {
        log_text = fmt::format(
            "[{}]::{}",
            fmt::format(fg(fmt::color(logColor[type])), "{}", type),
            log_text);
    }
    std::cout << log_text << std::endl;
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

void Logger::Logging::setWithDateTime(bool value)
{
    withDateTime = std::move(value);
}

void Logger::ThreadLogging::addLogToBuffer(const std::string &log_text)
{
    {
        std::lock_guard<std::mutex> lock(bufferMutex);
        logBuffer.push(log_text);
    }
    this->bufferCv.notify_one();
}

void Logger::ThreadLogging::processLogBuffer()
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
                logBuffer.pop();
                lock.unlock();
                withDateTime ? printLogWithDateTime(Logger::info_label, logBuffer.front()) : printLogWithoutDatetime(Logger::info_label, logBuffer.front());
                lock.lock();
            }
        }
    }
    catch (const std::exception &error)
    {
        this->sendError("Logger", "Empty", "Empty", "Empty", "Logger.readLogBuffer", error.what());
    }
}

void Logger::ThreadLogging::setFinished(bool value)
{
    finished = std::move(value);
}

void Logger::ThreadLogging::notifyBuffer()
{
    this->bufferCv.notify_one();
}