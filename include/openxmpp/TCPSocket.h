/**
* @author: Mital Vora <mital.d.vora@gmail.com>
*/

#ifndef _TCP_SOCKET_H_
#define _TCP_SOCKET_H_

#include "SocketExceptions.h"
#include "TCPSocketHandler.h"

#include <deque>

#include <asio.hpp>
#include <asio/detail/mutex.hpp>
using asio::ip::tcp;


#define BUFFER_SIZE 1024   // choose a reasonable size to send back to JS

class TCPSocket
{
public:
	TCPSocket(const std::string & hostname,
		const std::string& port,
		TCPSocketHandler * handler);
	virtual ~TCPSocket();

	void connect(long timeout = 10);
	void connectNB();
	bool write(const std::string &data);
	std::string read();
	bool isClosed();
	bool close();

	void setKeepAlive(bool keep_alives);
	//void setKeepAliveTimes(int inactivetime, int resendtime) ;

	static void initialize();
	static void uninitialize();

private:
	static asio::io_service io_service;
	static std::auto_ptr<asio::io_service::work> io_idlework;

	const std::string hostname;
	const std::string port;
	TCPSocketHandler * handler;

	bool non_blocking;
	bool keep_alives;
	//int inactivetime;
	//int resendtime;
	tcp::resolver resolver;
	tcp::socket socket;

	char read_data_buffer[BUFFER_SIZE + 1];

	asio::detail::mutex write_mutex;
	std::deque<std::string> write_buffer;

	enum SOCK_STATE_en { SOCK_CLOSED, SOCK_CONNECTING, SOCK_CONNECTED, SOCK_CLOSING } sock_state;

	void registerHandleResolve();
	void handleResolve(const asio::error_code& error, tcp::resolver::iterator endpoint_iterator);
	void registerHandleConnect(tcp::resolver::iterator endpoint_iterator);
	void handleConnect(const asio::error_code& error, tcp::resolver::iterator endpoint_iterator);

	void registerHandleRead();
	void handleRead(const asio::error_code& error, std::size_t bytes_transferred);

	void writeAsync(const std::string &data);
	void registerHandleWrite();
	void handleWrite(const asio::error_code& error, std::size_t bytes_transferred);

	void CompleteClose();

	void OnConnect();
	void OnRead(char * data, int size);
	void OnClose();
	void OnError(const std::string& error_text);
};

#endif
