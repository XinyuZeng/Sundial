//
// Created by Zhihan Guo on 4/12/20.
//

#include "message.h"


Message::Message(Type type, uint32_t dest, uint64_t txn_id, int size, char * data)
		: _msg_type(type)
		, _txn_id(txn_id)
		, _data_size(size)
		, _data(data)
{
	_dest_node_id = dest;
    Status       _status;
    uint64_t     _latest_lsn;
	//_src_node_id = g_node_id;
}

Message::Message(Type type, uint32_t dest, uint64_t txn_id, int size,
        char * data, Status stat, uint64_t lsn)
        : _msg_type(type)
        , _txn_id(txn_id)
        , _data_size(size)
        , _data(data)
        , _status(stat)
        , _latest_lsn(lsn)
{
    _dest_node_id = dest;
    _latest_lsn = 0;
    //_src_node_id = g_node_id;
}

Message::Message(Message * msg)
{
	memcpy(this, msg, sizeof(Message));
	_data = (char *) MALLOC(_data_size);
	memcpy(_data, msg->get_data(), _data_size);
}

Message::Message(char * packet)
{
	memcpy(this, packet, sizeof(Message));
	if (_data_size > 0) {
		_data = (char *) MALLOC(_data_size);
		memcpy(_data, packet + sizeof(Message), _data_size);
	} else
		_data = NULL;
}

Message::~Message()
{
	if (_data_size > 0)
		FREE(_data, _data_size);
}

uint32_t
Message::get_packet_len()
{
	return sizeof(Message) + _data_size;
}

void
Message::to_packet(char * packet)
{
	memcpy(packet, this, sizeof(Message));
	if (_data_size > 0)
		memcpy(packet + sizeof(Message), _data, _data_size);
    printf("data to send: %s\n", _data);
}

string
Message::get_name(Type type)
{
	switch(type) {
        case PREPARED:
            return "PREPARED";
        case COMMITTED:
            return "COMMITTED";
	    case ACK:
	        return "ACK";
		default:
		    assert(false);
	}
}

bool
Message::is_response(Type type)
{
	return (type == PREPARED)
		   || (type == COMMITTED)
		   || (type == ACK);
}
