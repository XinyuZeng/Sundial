#include "message.h"

class LogManager {
public:
	LogManager() {
		latest_LSN = 0;
	}
	Message * handleRequest(char * buffer) {
	    // Parse in message
        Message * msg = new Message(buffer);
        printf("receive data: txn-%lu %s\n", msg->get_txn_id(),msg->get_data());
        flushLog(msg);
        // Prepare out message
        msg->set_type(Message::ACK);
        msg->set_latest_lsn(latest_LSN);
        msg->set_status(check_txn_status());
        return msg;
	};

	void flushLog(Message * m) {
        latest_LSN++;
	};

	Message::Status check_txn_status() {
	    return Message::PREPARED_ABORT;
	};

private:
	uint64_t latest_LSN;

};
