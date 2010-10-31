/**
* @author: Mital Vora <mital.d.vora@gmail.com>
*/

#ifndef _SOCKET_EXCEPTIONS_H_
#define _SOCKET_EXCEPTIONS_H_

#include <exception>

class SocketException : public std::exception
{
public:
	virtual const char * what() const throw()
	{ return "Socket Exception"; }
};

class TCPSocketException : public SocketException
{
public:
	virtual const char * what() const throw()
	{ return "TCPSocket: Exception"; }
};

class TCPSocketConnectedException : public TCPSocketException
{
public:
	virtual const char * what() const throw()
	{ return "TCPSocket: either socket is connected or connecting."; }
};

class TCPSocketWriteException : public TCPSocketException
{
public:
	virtual const char * what() const throw()
	{ return "TCPSocket: socket is not open for write."; }
};

class TCPSocketReadException : public TCPSocketException
{
public:
	virtual const char * what() const throw()
	{ return "TCPSocket: socket is not open for read."; }
};

#endif
