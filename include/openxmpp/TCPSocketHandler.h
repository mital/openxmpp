/**
* @author: Mital Vora <mital.d.vora@gmail.com>
*/

#ifndef _TCP_SOCKET_HANDLER_H_
#define _TCP_SOCKET_HANDLER_H_

#include <string>

class TCPSocketHandler
{
public:
	TCPSocketHandler() {}
	virtual ~TCPSocketHandler() {}

	virtual void on_connect() = 0;
	virtual void on_read(char * data, int size) = 0;
	virtual void on_error(const std::string& error_text) = 0;
	virtual void on_close() = 0;
};

#endif
