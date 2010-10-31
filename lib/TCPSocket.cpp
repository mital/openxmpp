/**
* @author: Mital Vora <mital.d.vora@gmail.com>
*/

#ifndef WINVER
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif						

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0600
#endif

#include <boost/bind.hpp>
#include <openxmpp/TCPSocket.h>

asio::io_service TCPSocket::io_service;
std::auto_ptr<asio::io_service::work> TCPSocket::io_idlework(
	new asio::io_service::work(io_service));

void TCPSocket::initialize()
{
	asio::thread *io_thread = new asio::thread(
		boost::bind(&asio::io_service::run, &TCPSocket::io_service));
}

void TCPSocket::uninitialize()
{
}


TCPSocket::TCPSocket(const std::string& hostname,
					 const std::string& port,
					 TCPSocketHandler * handler)
: hostname(hostname),
port(port),
handler(handler),
non_blocking(false),
keep_alives(true),
//inactivetime(1),
//resendtime(1),
resolver(TCPSocket::io_service),
socket(TCPSocket::io_service),
sock_state(SOCK_CLOSED)
{
}

TCPSocket::~TCPSocket()
{
	this->CompleteClose();
}

bool TCPSocket::isClosed()
{
	return (this->sock_state == SOCK_CLOSED);
}

void TCPSocket::setKeepAlive(bool keep_alives)
{
	this->keep_alives = keep_alives;
	asio::socket_base::keep_alive option(this->keep_alives);
	socket.set_option(option);
}

//void TCPSocket::setKeepAliveTimes(int inactivetime, int resendtime) 
//{
//	if(this->sock_state != SOCK_CLOSED) 
//	{
//		throw TCPSocketConnectedException();
//	}
//	//this->inactivetime = inactivetime;
//	//this->resendtime = resendtime;
//}

void TCPSocket::connect(long timeout)
{
	// TODO: implement this method
	return;
	if(this->sock_state != SOCK_CLOSED)
	{
		throw TCPSocketConnectedException();
		return;
	}
	non_blocking = false;
	this->sock_state = SOCK_CONNECTING;

	try
	{
		//TODO: handle timeout for connect
		//tcp::resolver::iterator endpoint_iterator = ;
		//socket.connect(*endpoint_iterator);
	}
	catch(asio::system_error & e)
	{
		socket.close();
		this->OnError(e.what());
	}
}

void TCPSocket::registerHandleResolve()
{
	tcp::resolver::query query(hostname, port);
	resolver.async_resolve(query,
		boost::bind(&TCPSocket::handleResolve, this,
		asio::placeholders::error, asio::placeholders::iterator));
}

void TCPSocket::handleResolve(const asio::error_code& error,
							  tcp::resolver::iterator endpoint_iterator)
{
	if (error)
	{
		this->OnError(error.message());
		return;
	}
	registerHandleConnect(endpoint_iterator);
}


void TCPSocket::registerHandleConnect(tcp::resolver::iterator endpoint_iterator)
{
	if (endpoint_iterator != tcp::resolver::iterator())
	{
		socket.async_connect(*endpoint_iterator,
			boost::bind(&TCPSocket::handleConnect, this,
			asio::placeholders::error, ++endpoint_iterator));
		return;
	}
	this->OnError("TCPSocket Host resolution Error");
}

void TCPSocket::handleConnect(const asio::error_code& error,
							  tcp::resolver::iterator endpoint_iterator)
{
	if (!error)
	{
		this->OnConnect();
		this->registerHandleRead();
		return;
	}

	socket.close();
	if (endpoint_iterator != tcp::resolver::iterator())
	{
		this->registerHandleConnect(endpoint_iterator);
		return;
	}
	this->OnError(error.message());
}

void TCPSocket::connectNB()
{
	if(this->sock_state != SOCK_CLOSED)
	{
		throw TCPSocketConnectedException();
	}
	non_blocking = true;
	this->sock_state = SOCK_CONNECTING;
	this->registerHandleResolve();
}

void TCPSocket::registerHandleRead()
{
	asio::async_read(socket,
		asio::buffer(read_data_buffer, BUFFER_SIZE),
		asio::transfer_at_least(1),
		boost::bind(&TCPSocket::handleRead, this,
		asio::placeholders::error, asio::placeholders::bytes_transferred));
}

void TCPSocket::handleRead(const asio::error_code& error, std::size_t bytes_transferred)
{
	if (error)
	{
		this->OnError(error.message());
		return;
	}
	this->OnRead(read_data_buffer, bytes_transferred);
	this->registerHandleRead();
}

void TCPSocket::OnConnect() 
{
	this->sock_state = SOCK_CONNECTED;
	if (handler)
	{
		handler->on_connect();
	}
}

void TCPSocket::OnRead(char * read_data_buffer, int size) 
{
	read_data_buffer[size] = '\0';
	if (handler)
	{
		handler->on_read(read_data_buffer, size);
	}
}

void TCPSocket::OnError(const std::string& error_text) 
{
	this->CompleteClose();
	if (handler)
	{
		handler->on_error(error_text);
	}
}

void TCPSocket::OnClose()
{
	this->CompleteClose();
	if (handler)
	{
		handler->on_close();
	}
}

void TCPSocket::registerHandleWrite()
{
	asio::async_write(socket,
		asio::buffer(write_buffer.front().c_str(), write_buffer.front().size()),
		boost::bind(&TCPSocket::handleWrite, this,
		asio::placeholders::error, asio::placeholders::bytes_transferred));

}

void TCPSocket::handleWrite(const asio::error_code& error, std::size_t bytes_transferred)
{
	if (error)
	{
		this->OnError(error.message());
		return;
	}
	asio::detail::mutex::scoped_lock lock(write_mutex);
	write_buffer.pop_front();
	if (!write_buffer.empty())
	{
		this->registerHandleWrite();
	}
}

void TCPSocket::writeAsync(const std::string &data)
{
	asio::detail::mutex::scoped_lock lock(write_mutex);
	bool write_in_progress = !write_buffer.empty();
	write_buffer.push_back(data);
	if (!write_in_progress)
	{
		this->registerHandleWrite();
	}
}

bool TCPSocket::write(const std::string &data)
{
	if (this->sock_state != SOCK_CONNECTED)
	{
		throw TCPSocketWriteException();
	}
	if(non_blocking)
	{
		writeAsync(data);
		return true;
	}
	else
	{
		// TODO:
		return false;
	}
}

std::string TCPSocket::read()
{
	if (this->sock_state != SOCK_CONNECTED)
	{
		throw TCPSocketReadException();
	}
	// TODO: implement sync read
	return std::string("");
}

bool TCPSocket::close()
{
	if (this->sock_state != SOCK_CLOSED)
	{
		// Log  ->Debug("Closing socket to: %s:%d ", this->hostname.c_str(), this->port.c_str());
		this->CompleteClose();
		return true;
	}
	return false;
}

void TCPSocket::CompleteClose()
{
	if ((this->sock_state == SOCK_CONNECTED)
		|| (this->sock_state == SOCK_CONNECTING))
	{
		this->sock_state = SOCK_CLOSING;
		socket.close();
		this->sock_state = SOCK_CLOSED;
	}
}

