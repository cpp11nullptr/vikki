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

#ifndef VIKKI_AGENT_SENSOR_LOADER_H
#define VIKKI_AGENT_SENSOR_LOADER_H

#include "exception.h"
#include "sensor.h"

#include <string>
#include <vector>

namespace vikki
{
	class sensor_loader
	{
	public:
		sensor_loader();
		~sensor_loader();

		sensor_loader(const sensor_loader& loader) = delete;
		sensor_loader& operator=(const sensor_loader& loader) = delete;

		static sensor_loader& instance();

		size_t get_sensor_count() const;
		std::string get_sensor_name(size_t idx) const;

		sensor* get_sensor(const std::string& name) const;

	private:
		using create_sensor_t = sensor* (*)();
		using destroy_sensor_t = void (*)(sensor*);

		struct sensor_plugin
		{
			void *plugin_handle;
			sensor *sensor_handle;
			create_sensor_t create_sensor;
			destroy_sensor_t destroy_sensor;
		};

		const std::string _path;
		std::vector<sensor_plugin> _plugins;

		void load_sensors();
		void unload_sensors();

	};
}

#endif // VIKKI_AGENT_SENSOR_LOADER_H

