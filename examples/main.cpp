#include <iostream>
#include <Logger/Logger.hpp>

using namespace std;

int main()
{
    Logger::Logging logger("logs/log.txt", "10kb");
    logger.writeLog("Error","Hello World");
    logger.sendError("logs","44444444","33333333","2222222","21223333444","fdffdffdffd");
    logger.printLog(Logger::error_label,"red color of log text",true);
    logger.printLog(Logger::debug_label,"grey color of log text",true);
    logger.printLog(Logger::info_label,"blue color of log text",true);
    logger.printLog(Logger::success_label,"green color of log text",true);
    logger.printLog(Logger::warning_label,"orange color of log text",true);
    return 0;
}