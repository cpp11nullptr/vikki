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

#include "memory_usage_sensor.h"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>

namespace vikki
{
	memory_usage_sensor::memory_usage_sensor()
	{
	}

	memory_usage_sensor::~memory_usage_sensor()
	{
	}

	std::string memory_usage_sensor::name() const
	{
		return "memory_usage";
	}

	std::vector<char> memory_usage_sensor::data()
	{
		int fd = open("/proc/meminfo", O_RDONLY);
		if (fd < 0)
		{
			throw exception("Can't open /proc/meminfo, error code: " + std::to_string(errno));
		}

		char buffer[8192];

		int len = read(fd, buffer, sizeof(buffer));		
		if (len < 0)
		{
			throw exception("Can't read /proc/meminfo, error code: " + std::to_string(errno));
		}

		uint64_t total = parse_mem_param(buffer, "MemTotal:");
		uint64_t free = parse_mem_param(buffer, "MemFree:");

		uint64_t buffers = parse_mem_param(buffer, "Buffers:");
		uint64_t cached = parse_mem_param(buffer, "Cached:");

		uint64_t swap_total = parse_mem_param(buffer, "SwapTotal:");
		uint64_t swap_free = parse_mem_param(buffer, "SwapFree:");

		std::vector<char> result(sizeof(uint64_t) * 7);

		*reinterpret_cast<uint64_t*>(result.data()) = total;
		*reinterpret_cast<uint64_t*>(result.data() + sizeof(uint64_t)) = free;

		*reinterpret_cast<uint64_t*>(result.data() + sizeof(uint64_t) * 2) = buffers;
		*reinterpret_cast<uint64_t*>(result.data() + sizeof(uint64_t) * 3) = cached;

		*reinterpret_cast<uint64_t*>(result.data() + sizeof(uint64_t) * 4) = swap_total;
		*reinterpret_cast<uint64_t*>(result.data() + sizeof(uint64_t) * 5) = swap_free;

		int err = close(fd);
		if (err < 0)
		{
			throw exception("Can't close /proc/loadavg, error code: " + std::to_string(errno));
		}

		return result;
	}

	uint64_t memory_usage_sensor::parse_mem_param(char *buffer, const char *attr) const
	{
		int len = strlen(attr);

		uint64_t val = 0;
		char *ptr, *tok;

		if ((ptr = strstr(buffer, attr)))
		{
			ptr += len;
			val = strtoull(ptr, &tok, 0);

			while (*tok == ' ')
			{
				++tok;
			}

			if (*tok == 'k')
			{
				val *= 1024;
			}
			else if (*tok == 'M')
			{
				val *= (1024 * 1024);
			}
		}

		return val;
	}

	sensor *create_sensor()
	{
		return new memory_usage_sensor();
	}

	void destroy_sensor(sensor *handle)
	{
		delete handle;
	}
}
