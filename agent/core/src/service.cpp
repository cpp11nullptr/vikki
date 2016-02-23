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

#include "service.h"

#include "asio/signal_set.hpp"

#include <iostream>
#include <functional>

namespace vikki
{
	service::service()
		: _config("vikki-agent.cfg")
	{
		storage_loader::instance();
		sensor_loader::instance();
	}

	service::~service()
	{
	}

	void service::run()
	{
		init_storage();
		init_sensors();
		init_network();

		asio::signal_set signal_set(_service, SIGINT, SIGTERM);
		signal_set.async_wait(std::bind(&asio::io_service::stop, &_service));

		asio::steady_timer timer(_service);
		timer_tick(timer);

		_service.run();
	}

	void service::init_storage()
	{
		const config::storage_info& info = _config.storage();

		if (!info.enabled)
		{
			_active_storage = nullptr;

			return;
		}

		_active_storage = storage_loader::instance().get_storage(info.name);

		_active_storage->open(info.params);
	}

	void service::init_sensors()
	{		
		for (const config::sensor_info& sensor_info : _config.sensors())
		{
			if (!sensor_info.active)
			{
				continue;
			}

			sensor *sens = sensor_loader::instance().get_sensor(sensor_info.name);

			sens->init(sensor_info.params);

			if (_active_storage != nullptr)
			{
				_active_storage->prepare_entity(sensor_info.name);
			}
		}
	}

	void service::init_network()
	{
		config::network_info info = _config.network();

		if (!info.enabled)
		{
			_network = nullptr;

			return;
		}

		_network.reset(new network(_active_storage));

		if (info.security.enable)
		{
			if (info.security.type == "ssl")
			{
				_network->encryption(info.security.params);
			}
			else
			{
				throw exception("Unsupported network security type \"" + info.security.type + "\"");
			}
		}

		_network->start(info.address, info.port);
	}

	void service::timer_tick(asio::steady_timer& timer)
	{
		update_sensors();

		timer.expires_from_now(std::chrono::seconds(3));
		timer.async_wait(std::bind(&service::timer_tick, this, std::ref(timer)));
	}

	void service::update_sensors()
	{
		std::time_t time = std::time(nullptr);

		for (const config::sensor_info& sensor_info : _config.sensors())
		{
			if (!sensor_info.active)
			{
				continue;
			}

			try
			{
				sensor *sens = sensor_loader::instance().get_sensor(sensor_info.name);

				const std::string& sensor_name = sens->name();
				const std::vector<char>& sensor_data = sens->data();

				if (sensor_data.size() == 0)
				{
					continue;
				}

				if (_active_storage != nullptr)
				{
					_active_storage->put_data(sensor_name, time, sensor_data);
				}

				if (_network != nullptr)
				{
					_network->sensor_updated(sensor_name, time, sensor_data);
				}
			}
			catch (const std::exception& ex)
			{
				std::cerr << "Error occurred while updating sensors: " << ex.what() << "\n";
				std::cerr.flush();
			}
		}
	}
}
