#include "log.h"
#include "manager.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

LogManager::LogManager()
{
    _buffer_size = 64 * 1024 * 1024;
    _buffer = new char[_buffer_size]; // 64 MB
    _lsn = 0;
}

LogManager::LogManager(char * log_name)
{
    _buffer_size = 64 * 1024 * 1024;
    _buffer = new char[_buffer_size]; // 64 MB
    _lsn = 0;
    _name_size = 50;
    _log_name = new char[_name_size];
    strcpy(_log_name, log_name);
    //TODO: delete O_TRUNC when recovery is needed.
    _log_fd = open(log_name, O_RDWR | O_CREAT | O_TRUNC | O_DIRECT | O_APPEND, 0755);
    if (_log_fd == 0) {
        perror("open log file");
        exit(1);
    }
}

LogManager::~LogManager() {
		delete[] _log_name;
		_log_name = nullptr;
		close(_log_fd);
}

void
LogManager::log(uint32_t size, char * record)
{
    uint32_t lsn = ATOM_ADD(_lsn, size);
    uint32_t start = lsn % _buffer_size;
    if (lsn / _buffer_size == (lsn + size) / _buffer_size) {
        memcpy(_buffer + start, record, size);
    } else {
        uint32_t tail_size = _buffer_size - start;
        memcpy(_buffer + start, record, tail_size);
        memcpy(_buffer, record + tail_size, size - tail_size);
    }
    INC_FLOAT_STATS(log_size, size);
    // TODO should write buffer to disk. For now, assume NVP or battery backed DRAM.
    if (write(_log_fd, record, size) == -1) {
			perror("write");
			exit(1);
    }
    if (fsync(_log_fd) == -1) {
        perror("fsync");
        exit(1);
    }
}
