/*

The MIT License (MIT)

Copyright (c) 2016 Ievgen Polyvanyi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef VIKKI_AGENT_NETWORK_H
#define VIKKI_AGENT_NETWORK_H

#include "storage.h"

#include "network/server.h"

#include <memory>
#include <vector>
#include <exception>

namespace vikki
{
	class network
	{
	public:
		network(storage *store);
		~network();

		void encryption(const std::map<std::string, std::string>& params);

		void start(const std::string& address, int port);
		void stop();

		void sensor_updated(const std::string& name, std::time_t time, const std::vector<char>& data);

		void sensor_change_subscription(std::shared_ptr<lnetlib::connection> conn, std::unique_ptr<lnetlib::istream> stream);
		void sensor_get_data(std::shared_ptr<lnetlib::connection> conn, std::unique_ptr<lnetlib::istream> stream);
		void sensor_get_list(std::shared_ptr<lnetlib::connection> conn, std::unique_ptr<lnetlib::istream> stream);

	private:
		struct sensor_subscriber
		{
			std::string sensor_name;
			std::shared_ptr<lnetlib::connection> conn;
		};

		storage *_storage;
		lnetlib::server _server;
		std::vector<sensor_subscriber> _sensor_subscribers;

		void connected(std::shared_ptr<lnetlib::connection> conn);
		void disconnected(std::shared_ptr<lnetlib::connection> conn);

		void received(std::shared_ptr<lnetlib::connection> conn, std::unique_ptr<lnetlib::istream> stream);
		void error(std::shared_ptr<lnetlib::connection> conn, int code, const std::string& message);

	};
}

#endif // VIKKI_AGENT_NETWORK_H

