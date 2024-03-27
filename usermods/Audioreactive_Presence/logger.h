#ifndef LOGGER_H
#define LOGGER_H
class Logger
{
    public:
    Logger(bool isDebug);
    void Log(const char *message);
    private: 
        bool isDebug;
};
#endif // LOGGER_H