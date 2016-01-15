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

#ifndef VIKKI_AGENT_STORAGE_LOADER_H
#define VIKKI_AGENT_STORAGE_LOADER_H

#include "exception.h"
#include "storage.h"

#include <string>
#include <vector>
#include <map>

namespace vikki
{
	class storage_loader
	{
	public:
		storage_loader();
		~storage_loader();

		storage_loader(const storage_loader& loader) = delete;
		storage_loader& operator=(const storage_loader& loader) = delete;

		static storage_loader& instance();

		storage* get_storage(const std::string& name);

	private:
		using create_storage_t = storage* (*)();
		using destroy_storage_t = void (*)(storage*);

		struct storage_plugin
		{
			void *plugin_handle;
			storage *storage_handle;
			create_storage_t create_storage;
			destroy_storage_t destroy_storage;
		};

		const std::string _path;
		std::vector<storage_plugin> _plugins;

		void load_storages();
		void unload_storages();

	};
}

#endif // VIKKI_AGENT_STORAGE_LOADER_H

