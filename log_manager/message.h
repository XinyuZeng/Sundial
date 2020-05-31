//
// Created by Zhihan Guo on 4/12/20.
//

#ifndef SUNDIAL_MESSAGE_H
#define SUNDIAL_MESSAGE_H

#include <string>
#include "helper.h"

using namespace std;

class Message
{
public:
	enum Type {
		VOTE_REQ, // used to check vote & log vote
		COMMIT_REQ, // used to check commit status & log commit
		ABORT_REQ, // used to abort
		ACK
	};
	enum Status {
	    PREPARED_COMMIT,
	    PREPARED_ABORT,
	    COMMIT
	};
	Message(Type type, uint32_t dest, uint64_t txn_id, int size, char * data);
    Message(Type type, uint32_t dest, uint64_t txn_id, int size, char * data,
            Status stat, uint64_t lsn);
	Message(Message * msg);
	Message(char * packet);
	~Message();

	uint32_t get_packet_len();
	uint32_t get_dest_id()         { return _dest_node_id; }
	void set_dest_id(uint32_t dest)    { _dest_node_id = dest; }
	uint32_t get_src_node_id()    { return _src_node_id; }

	uint32_t get_data_size()    { return _data_size; }
	void set_data(char * data)  { _data = data; }
	uint32_t get_data(char * &data)    { data = _data; return _data_size; }
	char * get_data()            { return _data; }
	Type get_type()             { return _msg_type; }
	void set_type(Type type)     { _msg_type = type; }
	uint64_t get_txn_id()        { return _txn_id; }
    uint64_t get_status()        { return _status; }
    void set_status(Status s)        { _status = s; }
    uint64_t get_latest_lsn()           { return _latest_lsn; }
    void set_latest_lsn(uint64_t lsn)           { _latest_lsn = lsn; }

	void to_packet(char * packet);
	static string get_name(Type type);
	static bool is_response(Type type);
	string get_name() { return get_name(get_type()); }
	bool is_response() { return is_response(get_type()); }
private:
	Type         _msg_type;
	uint32_t     _src_node_id;
	uint32_t     _dest_node_id;
	uint64_t     _txn_id;
	Status       _status;
    uint64_t     _latest_lsn;

	uint32_t    _data_size;
	char *         _data;
};
#endif //SUNDIAL_MESSAGE_H
