#pragma once

#include <string>
#include <type_traits>
#include <functional>
#include <map>

#include "io_context.h"
#include "acceptor.h"
#include "connector.h"

namespace quark
{
	using namespace std;

	class ConnectionTrait
	{
	public:
		int indentify();
		void handle_read();
		void handle_write();
	};

	class Service
	{
	public:
		Service(IoContext *ctx, string addr, int port) : addr_(addr), port_(port), ctx_(ctx), acceptor_(ctx_, 100) {
		}
		~Service() {}

		void handle_new_connection();

		bool Start()
		{
			if (!acceptor_.Open(port_))
			{
				return false;
			}

			if (!acceptor_.Accept())
			{
				return false;
			}
			cout << "server_socket begin listen: " << acceptor_.GetSocket() << endl;
			return true;
		}
		void Run()
		{
			try
			{
                ctx_->Run();
			}
			catch (exception e)
			{
				cout << "error happened : " << e.what() << endl;
				cerr << "errno: " << errno << endl;
			}
		}

	private:
		/* data */
		string addr_;
		int port_;
		IoContext *ctx_;
		map<int, Connector> conns_;

		Acceptor acceptor_;
	};
}