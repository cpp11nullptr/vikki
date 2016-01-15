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

#ifndef VIKKI_AGENT_CONFIG_H
#define VIKKI_AGENT_CONFIG_H

#include "exception.h"
#include "json.h"

#include <string>
#include <map>
#include <list>

namespace vikki
{
	class config
	{
	public:
		struct storage_info
		{
			bool enabled;
			std::string name;
			std::map<std::string, std::string> params;
		};

		struct sensor_info
		{
			bool active;
			std::string name;
		};

		struct network_security_info
		{
			bool enable;
			std::string type;
			std::map<std::string, std::string> params;
		};

		struct network_info
		{
			bool enabled;
			std::string address;
			int port;
			network_security_info security;
		};

		config(const std::string& filename);
		~config();

		storage_info storage() const;
		std::list<sensor_info> sensors() const;
		network_info network() const;

	private:
		network_info _network;
		storage_info _storage;
		std::list<sensor_info> _sensors;

		void load_network_settings(const nlohmann::json& node);
		void load_storage_settings(const nlohmann::json& node);
		void load_sensors_settings(const nlohmann::json& node);

	};
}

#endif // VIKKI_AGENT_CONFIG_H

