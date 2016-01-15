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

#include "sensor_loader.h"

#include <iostream>
#include <string.h>
#include <dirent.h>
#include <dlfcn.h>

namespace vikki
{
	sensor_loader::sensor_loader()
		: _path("./sensors/")
	{
		load_sensors();
	}

	sensor_loader::~sensor_loader()
	{
		unload_sensors();
	}

	sensor_loader& sensor_loader::instance()
	{
		static sensor_loader loader;

		return loader;
	}

	size_t sensor_loader::get_sensor_count() const
	{
		return _plugins.size();
	}

	std::string sensor_loader::get_sensor_name(size_t idx) const
	{
		if (idx >= _plugins.size())
		{
			throw exception("Can't get sensor name: out of index");
		}

		return _plugins.at(idx).sensor_handle->name();
	}

	sensor* sensor_loader::get_sensor(const std::string& name) const
	{
		sensor *sensor_handle = nullptr;

		for (const sensor_plugin& plugin : _plugins)
		{
			if (plugin.sensor_handle->name() == name)
			{
				sensor_handle = plugin.sensor_handle;

				break;
			}
		}

		if (sensor_handle == nullptr)
		{
			throw exception("Can't get sensor \"" + name + "\": sensor doesn't exist");
		}

		return sensor_handle;
	}

	void sensor_loader::load_sensors()
	{
		DIR *dir = opendir(_path.c_str());
		if (dir == nullptr)
		{
			if (_path.size() == 0 || errno != ENOENT)
			{
				throw exception("Can't open sensor plugins directory, error code: " + std::to_string(errno));
			}

			return;
		}

		for (dirent *entry = readdir(dir); entry != nullptr; entry = readdir(dir))
		{
			if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
			{
				continue;
			}

			sensor_plugin plugin;

			const std::string filename = _path + entry->d_name;

			plugin.plugin_handle = dlopen(filename.c_str(), RTLD_LAZY);
			if (plugin.plugin_handle == nullptr)
			{
				closedir(dir);

				throw exception("Load sensor plugin \"" + filename + "\" failed: " + std::string(dlerror()));
			}

			plugin.create_sensor = reinterpret_cast<create_sensor_t>(dlsym(plugin.plugin_handle, "create_sensor"));
			if (plugin.create_sensor == nullptr)
			{
				dlclose(plugin.plugin_handle);
				closedir(dir);

				throw exception("Lookup \"create_sensor\" function in sensor plugin \"" + filename + "\" failed: " + std::string(dlerror()));
			}

			plugin.destroy_sensor = reinterpret_cast<destroy_sensor_t>(dlsym(plugin.plugin_handle, "destroy_sensor"));
			if (plugin.destroy_sensor == nullptr)
			{
				dlclose(plugin.plugin_handle);
				closedir(dir);

				throw exception("Lookup \"destroy_sensor\" function in sensor plugin \"" + filename + "\" failed: " + std::string(dlerror()));
			}

			plugin.sensor_handle = plugin.create_sensor();

			_plugins.push_back(plugin);
		}

		closedir(dir);
	}

	void sensor_loader::unload_sensors()
	{
		for (const sensor_plugin& plugin : _plugins)
		{
			plugin.destroy_sensor(plugin.sensor_handle);

			dlclose(plugin.plugin_handle);
		}

		_plugins.clear();
	}
}
