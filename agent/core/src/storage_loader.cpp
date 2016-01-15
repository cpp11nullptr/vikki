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

#include "storage_loader.h"

#include <iostream>
#include <string.h>
#include <dirent.h>
#include <dlfcn.h>

namespace vikki
{
	storage_loader::storage_loader()
		: _path("./storages/")
	{
		load_storages();
	}

	storage_loader::~storage_loader()
	{
		unload_storages();
	}

	storage_loader& storage_loader::instance()
	{
		static storage_loader loader;

		return loader;
	}

	storage* storage_loader::get_storage(const std::string& name)
	{
		storage *storage_handle = nullptr;

		for (const storage_plugin& plugin : _plugins)
		{
			if (plugin.storage_handle->name() == name)
			{
				storage_handle = plugin.storage_handle;

				break;
			}
		}

		if (storage_handle == nullptr)
		{
			throw exception("Can't get storage \"" + name + "\": storage doesn't exist");
		}

		return storage_handle;
	}

	void storage_loader::load_storages()
	{
		DIR *dir = opendir(_path.c_str());
		if (dir == nullptr)
		{
			if (_path.size() == 0 || errno != ENOENT)
			{
				throw exception("Can't open storage plugins directory, error code: " + std::to_string(errno));
			}

			return;
		}

		for (dirent *entry = readdir(dir); entry != nullptr; entry = readdir(dir))
		{
			if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0)
			{
				continue;
			}

			storage_plugin plugin;

			const std::string filename = _path + entry->d_name;

			plugin.plugin_handle = dlopen(filename.c_str(), RTLD_LAZY);
			if (plugin.plugin_handle == nullptr)
			{
				closedir(dir);

				throw exception("Load storage plugin \"" + filename + "\" failed: " + std::string(dlerror()));
			}

			plugin.create_storage = reinterpret_cast<create_storage_t>(dlsym(plugin.plugin_handle, "create_storage"));
			if (plugin.create_storage == nullptr)
			{
				dlclose(plugin.plugin_handle);
				closedir(dir);

				throw exception("Lookup \"create_storage\" function in storage plugin \"" + filename + "\" failed: " + std::string(dlerror()));
			}

			plugin.destroy_storage = reinterpret_cast<destroy_storage_t>(dlsym(plugin.plugin_handle, "destroy_storage"));
			if (plugin.destroy_storage == nullptr)
			{
				dlclose(plugin.plugin_handle);
				closedir(dir);

				throw exception("Lookup \"destroy_storage\" function in storage plugin \"" + filename + "\" failed: " + std::string(dlerror()));
			}

			plugin.storage_handle = plugin.create_storage();

			_plugins.push_back(plugin);
		}

		closedir(dir);
	}

	void storage_loader::unload_storages()
	{
		for (const storage_plugin& plugin : _plugins)
		{
			plugin.destroy_storage(plugin.storage_handle);

			dlclose(plugin.plugin_handle);
		}

		_plugins.clear();
	}
}
