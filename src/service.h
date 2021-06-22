#pragma once

#include <string>
#include <type_traits>
#include <functional>
#include <map>

#include "io_context.h"

using namespace std;

class ConnectionTrait {
public:
	int indentify();
	void handle_read();
	void handle_write();
};

template<typename T, typename std::enable_if<std::is_base_of<ConnectionTrait,T>::value>::type>
class Service
{
public:
	Service(io_context* ctx, string addr, int port): m_addr(addr), m_port(port), m_ctx(ctx) {}
	~Service() {}

	void handle_new_connection();

	bool start();
	void run(); 
private:
	/* data */
	string m_addr;
	int m_port;
	io_context* m_ctx;
	map<int, T> m_conns;
};