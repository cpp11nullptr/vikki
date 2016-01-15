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

#ifndef VIKKI_AGENT_SERVICE_H
#define VIKKI_AGENT_SERVICE_H

#include "config.h"
#include "sensor_loader.h"
#include "storage_loader.h"
#include "network.h"

#include "asio/io_service.hpp"
#include "asio/steady_timer.hpp"

#include <memory>

namespace vikki
{
	class service
	{
	public:
		service();
		~service();

		void run();

	private:
		config _config;
		std::unique_ptr<network> _network;
		storage *_active_storage;

		void init_storage();
		void init_sensors();
		void init_network();

		asio::io_service _service;

		void timer_tick(asio::steady_timer& timer);
		void update_sensors();

	};
}

#endif // VIKKI_AGENT_SERVICE_H

