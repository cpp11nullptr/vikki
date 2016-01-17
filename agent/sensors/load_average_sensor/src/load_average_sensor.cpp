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

#include "load_average_sensor.h"

#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#include <cstdlib>
#include <cerrno>

#include <cstdlib>
#include <cerrno>

namespace vikki
{
	load_average_sensor::load_average_sensor()
	{
	}

	load_average_sensor::~load_average_sensor()
	{
	}

	std::string load_average_sensor::name() const
	{
		return "load_average";
	}

	std::vector<char> load_average_sensor::data()
	{
		int fd = open("/proc/loadavg", O_RDONLY);
		if (fd < 0)
		{
			throw exception("Can't open /proc/loadavg, error code: " + std::to_string(errno));
		}

		char buffer[8192];
		char *pbuff = buffer;

		int len = read(fd, buffer, sizeof(buffer));
		if (len < 0)
		{
			close(fd);

			throw exception("Can't read /proc/loadavg, error code: " + std::to_string(errno));
		}

		double la1 = strtod(pbuff, &pbuff);
		double la5 = strtod(pbuff, &pbuff);
		double la15 = strtod(pbuff, &pbuff);

		std::vector<char> result(sizeof(double) * 3);

		void *ptr = result.data();

		write_data<double>(ptr, la1, &ptr);
		write_data<double>(ptr, la5, &ptr);
		write_data<double>(ptr, la15, &ptr);

		int err = close(fd);
		if (err < 0)
		{
			throw exception("Can't close /proc/loadavg, error code: " + std::to_string(errno));
		}

		return result;
	}

	sensor *create_sensor()
	{
		return new load_average_sensor();
	}

	void destroy_sensor(sensor *handle)
	{
		delete handle;
	}
}
