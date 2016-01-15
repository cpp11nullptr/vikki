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

#include "network.h"
#include "exception.h"
#include "command.h"
#include "sensor_loader.h"

#include <iostream>

namespace vikki
{
	network::network(storage *store)
		: _storage(store)
	{
	}

	network::~network()
	{
	}

	void network::encryption(const std::map<std::string, std::string>& params)
	{
		auto encrypt = _server.encryption_info();

		encrypt->init(lnetlib::encryption_method::tlsv11);

		auto iter = params.find("ca");
		if (iter != params.end())
		{
			encrypt->set_ca_filename(iter->second);
			encrypt->set_verify_mode(lnetlib::ssl_verify_peer | lnetlib::ssl_verify_fail_if_no_peer_cert);
		}

		iter = params.find("cert");
		if (iter != params.end())
		{
			encrypt->set_cert_filename(iter->second);
		}

		iter = params.find("private_key");
		if (iter != params.end())
		{
			encrypt->set_private_key_filename(iter->second);
		}

		encrypt->set_enabled(true);
	}

	void network::start(const std::string& address, int port)
	{
		_server.connected.connect(this, &network::connected);
		_server.disconnected.connect(this, &network::disconnected);
		_server.received.connect(this, &network::received);
		_server.error.connect(this, &network::error);

		_server.start(address, port);
	}

	void network::stop()
	{
		_server.stop();
	}

	void network::sensor_updated(const std::string& name, std::time_t time, const std::vector<char>& data)
	{
		for (auto iter = _sensor_subscribers.cbegin(); iter != _sensor_subscribers.cend(); ++iter)
		{
			const sensor_subscriber& subscriber = *iter;

			if (subscriber.sensor_name == name)
			{
				std::unique_ptr<lnetlib::ostream> stream = subscriber.conn->create_stream(command::sensor_data_updated);

				stream->write_string(name);
				stream->write_int64(time);
				stream->write_data_chunk(data);
			}
		}
	}

	void network::sensor_change_subscription(std::shared_ptr<lnetlib::connection> conn, std::unique_ptr<lnetlib::istream> stream)
	{
		const std::string& sensor_name = stream->read_string();
		const bool subscribe = stream->read_uint8() > 0 ? true : false;

		auto iter = _sensor_subscribers.begin();

		for ( ; iter != _sensor_subscribers.end(); ++iter)
		{
			const sensor_subscriber& subscriber = *iter;

			if (subscriber.sensor_name == sensor_name && subscriber.conn == conn)
			{
				break;
			}
		}

		if (subscribe && iter == _sensor_subscribers.end())
		{
			sensor_subscriber subscriber;

			subscriber.sensor_name = sensor_name;
			subscriber.conn = conn;

			_sensor_subscribers.push_back(subscriber);
		}
		else if (!subscribe && iter != _sensor_subscribers.end())
		{
			_sensor_subscribers.erase(iter);
		}
	}

	void network::sensor_get_data(std::shared_ptr<lnetlib::connection> conn, std::unique_ptr<lnetlib::istream> stream)
	{
		(void)conn;

		std::unique_ptr<lnetlib::ostream> response = stream->create_response();

		const std::string& sensor_name = stream->read_string();
		const std::time_t from = stream->read_int64();
		const std::time_t to = stream->read_int64();

		if (_storage != nullptr)
		{
			const storage::sensor_data_t& sensor_data = _storage->get_data(sensor_name, from, to);

			response->write_uint64(sensor_data.size());

			for (const std::pair<std::time_t, std::vector<char>>& iter : sensor_data)
			{
				response->write_int64(iter.first);
				response->write_data_chunk(iter.second);
			}
		}
		else
		{
			response->write_uint64(0);
		}
	}

	void network::sensor_get_list(std::shared_ptr<lnetlib::connection> conn, std::unique_ptr<lnetlib::istream> stream)
	{
		(void)conn;

		std::unique_ptr<lnetlib::ostream> response = stream->create_response();

		uint64_t count = sensor_loader::instance().get_sensor_count();
		response->write_uint64(count);

		for (uint64_t i = 0; i < count; ++i)
		{
			std::string name = sensor_loader::instance().get_sensor_name(i);
			response->write_string(name);
		}
	}

	void network::connected(std::shared_ptr<lnetlib::connection> conn)
	{
		(void)conn;
	}

	void network::disconnected(std::shared_ptr<lnetlib::connection> conn)
	{
		for (auto iter = _sensor_subscribers.begin() ; iter != _sensor_subscribers.end(); )
		{
			const sensor_subscriber& subscriber = *iter;

			if (subscriber.conn == conn)
			{
				iter = _sensor_subscribers.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}

	void network::received(std::shared_ptr<lnetlib::connection> conn, std::unique_ptr<lnetlib::istream> stream)
	{
		switch (stream->command<command>())
		{
		case command::sensor_data_subscribe:
			sensor_change_subscription(conn, std::move(stream));
			break;

		case command::get_sensor_data:
			sensor_get_data(conn, std::move(stream));
			break;

		case command::get_sensor_list:
			sensor_get_list(conn, std::move(stream));
			break;

		default:
			break;

		}
	}

	void network::error(std::shared_ptr<lnetlib::connection> conn, int code, const std::string& message)
	{
		(void)conn;
		(void)code;

		std::cerr << "Network error: " << message << "\n";
		std::cerr.flush();
	}
}
