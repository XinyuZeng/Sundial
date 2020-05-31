#include "message.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "log_record.h"

#define LOG_BUFFER_SIZE 512

class LogManager {
public:
	LogManager(char *log_name) {
		latest_LSN = 0;
		log_buffer = new char[LOG_BUFFER_SIZE];
		log_fd = open(log_name, O_RDWR | O_CREAT | O_TRUNC | O_DIRECT | O_APPEND);
		if (log_fd == 0) {
			perror("open log file");
			exit(1);
		}
	}

	~LogManager() {
		delete[] log_buffer;
		log_buffer = nullptr;
  	}

	Message * handleRequest(char * buffer) {
	    // Parse in message
        Message * msg = new Message(buffer);
        printf("receive data: txn-%lu %s\n", msg->get_txn_id(),msg->get_data());

		Message::Status txn_status = check_txn_status();
		if (msg->get_type() == Message::Type::VOTE_REQ) {
			if (txn_status == Message::Status::PREPARED_COMMIT) {
				// vote yes
				LogRecord log{msg->get_dest_id(), msg->get_txn_id(), 
					latest_LSN++, LogRecord::LogRecordType::YES};
				append_log(log);
			} else {
				// vote no?
				LogRecord log{msg->get_dest_id(), msg->get_txn_id(), 
					latest_LSN++, LogRecord::LogRecordType::ABORT};
				append_log(log);
			}
		} else if (msg->get_type() == Message::Type::COMMIT_REQ) {
			// commit 
			LogRecord log{msg->get_dest_id(), msg->get_txn_id(), 
					latest_LSN++, LogRecord::LogRecordType::COMMIT};
			append_log(log);
		} else if (msg->get_type() == Message::Type::ABORT_REQ) {
			// abort
			LogRecord log{msg->get_dest_id(), msg->get_txn_id(), 
					latest_LSN++, LogRecord::LogRecordType::ABORT};
			append_log(log);
		} else {
			assert(false);
		}

        // Prepare out message
        msg->set_type(Message::ACK);
        msg->set_latest_lsn(latest_LSN);
        msg->set_status(check_txn_status());
        return msg;
	};

	void flushLog(Message * m) {
        latest_LSN++;
	};

	void append_log(LogRecord &log_record) {
		memcpy(log_buffer, &log_record, sizeof(log_record));
		if (write(log_fd, log_buffer, sizeof(log_record)) == -1) {
			perror("write");
			exit(1);
		}
		if (fsync(log_fd) == -1) {
			perror("fsync");
			exit(1);
		}
	}

	Message::Status check_txn_status() {
	    return Message::PREPARED_ABORT;
	};

private:
	uint64_t latest_LSN;
	int log_fd;
	char *log_buffer;
};
