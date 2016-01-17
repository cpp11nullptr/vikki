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

#include "config.h"

#include <fstream>

namespace vikki
{
	config::config(const std::string& filename)
	{
		std::ifstream stream;
		stream.open(filename, std::ios::in);

		if (!stream.is_open())
		{
			throw exception("Can't open config file");
		}

		const nlohmann::json& data = nlohmann::json::parse(stream);

		load_storage_settings(data["storage"]);
		load_sensors_settings(data["sensors"]);
		load_network_settings(data["network"]);
	}

	config::~config()
	{
	}

	config::storage_info config::storage() const
	{
		return _storage;
	}

	std::list<config::sensor_info> config::sensors() const
	{
		return _sensors;
	}

	config::network_info config::network() const
	{
		return _network;
	}

	void config::load_storage_settings(const nlohmann::json& node)
	{
		if (node.is_null())
		{
			_storage.enabled = false;

			return;
		}

		_storage.enabled = true;

		_storage.name = node["name"].get<std::string>();

		nlohmann::json params_node = node["params"];
		if (!params_node.is_null())
		{
			const std::map<std::string, nlohmann::json>& params = params_node;

			for (const std::pair<std::string, nlohmann::json>& iter : params)
			{
				_storage.params[iter.first] = iter.second.get<std::string>();
			}
		}
	}

	void config::load_sensors_settings(const nlohmann::json& node)
	{
		for (const nlohmann::json& sensor : node)
		{
			sensor_info info;

			info.active = sensor["active"].get<bool>();
			info.name = sensor["name"].get<std::string>();

			nlohmann::json params_node = sensor["params"];
			if (!params_node.is_null())
			{
				const std::map<std::string, nlohmann::json>& params = params_node;

				for (const std::pair<std::string, nlohmann::json>& iter : params)
				{
					info.params[iter.first] = iter.second.get<std::string>();
				}
			}

			_sensors.push_back(info);
		}
	}

	void config::load_network_settings(const nlohmann::json& node)
	{
		if (node.is_null())
		{
			_network.enabled = false;

			return;
		}

		_network.enabled = true;

		_network.address = node["address"].get<std::string>();
		_network.port = node["port"].get<int>();

		_network.security.enable = false;

		nlohmann::json security_node = node["security"];
		if (!security_node.is_null())
		{
			_network.security.enable = security_node["enable"].get<bool>();
			_network.security.type = security_node["type"].get<std::string>();

			const std::map<std::string, nlohmann::json>& params = security_node["params"];

			for (const std::pair<std::string, nlohmann::json>& iter : params)
			{
				_network.security.params[iter.first] = iter.second.get<std::string>();
			}
		}
	}
}
