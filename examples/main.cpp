#include <iostream>
#include <Logger.hpp>

using namespace std;

int main()
{
    Logger::Logging logger("logs/log.txt", "10kb");
    logger.writeLog("Error","Hello World");
    logger.sendError("logs","44444444","33333333","2222222","21223333444","fdffdffdffd");
    logger.printLog("Error","Error: red color of log text");
    return 0;
}