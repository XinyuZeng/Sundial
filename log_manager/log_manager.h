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
		strcpy(this->log_name, log_name);
		log_fd = open(log_name, O_RDWR | O_CREAT | O_TRUNC | O_DIRECT | O_APPEND);
		if (log_fd == 0) {
			perror("open log file");
			exit(1);
		}
	}

	~LogManager() {
		delete[] log_buffer;
		log_buffer = nullptr;
		close(log_fd);
  	}

	/**
	 * @desc: This function is intended to called by a participant to handle 
	 * the request send by coordinator and return its vote.
	 * @input: Message from coordinator asking for vote/commit/abort
	 * @return: ACK: message request was successfully handled
	 * 			others: the vote already in log, request denied by insert-once
	**/
	Message * handleRequest(char * buffer) {
	    // Parse in message
        Message * msg = new Message(buffer);
        printf("receive data: txn-%lu %s\n", msg->get_txn_id(),msg->get_data());

		LogRecord::Type vote = LogRecord::INVALID;
		Message::Status txn_status = check_txn_status();
		if (msg->get_type() == Message::Type::VOTE_REQ) {
			// check whether already vote
			vote = check_log(msg);
			if (vote == LogRecord::INVALID) {
				if (txn_status == Message::Status::PREPARED_COMMIT) {
					// vote yes
					vote == LogRecord::YES;
					log_message(msg, LogRecord::Type::YES);
				} else {
					// vote no
					vote == LogRecord::ABORT;
					log_message(msg, LogRecord::Type::ABORT);
				}
			}
		} else if (msg->get_type() == Message::Type::COMMIT_REQ) {
			// commit 
			assert(check_log(msg) == LogRecord::YES);
			vote = LogRecord::Type::COMMIT;
			log_message(msg, LogRecord::Type::COMMIT);
		} else if (msg->get_type() == Message::Type::ABORT_REQ) {
			// abort
			vote = check_log(msg);
			if (vote != LogRecord::ABORT) {
				vote = LogRecord::ABORT;
				log_message(msg, LogRecord::Type::ABORT);
			}
		} else {
			assert(false);
		}

        // Prepare out message
        msg->set_type(log_to_message(vote));
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
	char log_name[50];

	void log_message(Message *msg, LogRecord::Type type) {
		latest_LSN = ATOM_FETCH_ADD(latest_LSN, 1);
		LogRecord log{msg->get_dest_id(), msg->get_txn_id(), 
					latest_LSN, type};
		append_log(log);
	};

	LogRecord::Type check_log(Message * msg) {
		LogRecord::Type vote = LogRecord::INVALID;
		FILE * fp = fopen(log_name, "r");
		fseek(fp, 0, SEEK_END);
		fseek(fp, -sizeof(LogRecord), SEEK_CUR);
		LogRecord cur_log;
		fread((void *)&cur_log, sizeof(LogRecord), 1, fp);

		while (cur_log.get_latest_lsn() > msg->get_latest_lsn()) {
			//TODO: whether to add log type equals?
			if (cur_log.get_txn_id() == msg->get_txn_id()) {
				// log exists
				vote = cur_log.get_log_record_type();
				break;
			}

			if (fseek(fp, -2 * sizeof(LogRecord), SEEK_CUR) == -1)
				break;
			fread((void *)&cur_log, sizeof(LogRecord), 1, fp);
		}
		
		fclose(fp);
		return vote;
	};

	Message::Type log_to_message(LogRecord::Type vote) {
		switch (vote)
		{
		case LogRecord::INVALID :
			return Message:: ACK;
		case LogRecord::COMMIT :
			return Message:: COMMITTED;
		case LogRecord::ABORT :
			return Message:: ABORTTED;
		case LogRecord::YES :
			return Message:: VOTED_YES;
		default:
			assert(false);
		}
	}
};
