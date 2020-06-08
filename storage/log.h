#pragma once
#include "global.h"
#include "helper.h"

class LogManager {
public:
    LogManager();
    LogManager(char *log_name);
    void log(uint32_t size, char * record);
private:
    uint32_t _buffer_size;
    char * _buffer;
    uint32_t _lsn;
    int _log_fd;
    uint32_t _name_size;
    char * _log_name;
};
