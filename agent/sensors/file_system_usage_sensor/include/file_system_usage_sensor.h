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

#ifndef VIKKI_FILE_SYSTEM_USAGE_SENSOR_H
#define VIKKI_FILE_SYSTEM_USAGE_SENSOR_H

#include "exception.h"
#include "sensor.h"

namespace vikki
{
	class file_system_usage_sensor
		: public sensor
	{
	public:
		file_system_usage_sensor();
		~file_system_usage_sensor() override;

		std::string name() const override;
		std::vector<char> data() override;

	private:
		struct file_system
		{
			std::string dir;
			std::string device;
			std::string type;
			std::string options;
		};

		struct file_system_info
		{
			uint64_t total;
			uint64_t free;
			uint64_t avail;
			uint64_t files;
			uint64_t free_files;
		};

		uint64_t blocks_to_bytes(uint64_t value, uint64_t bsize) const;

		std::vector<file_system> get_file_system_list() const;
		file_system_info get_file_system_info(const std::string& dir) const;

	};

	extern "C"
	{
		sensor* create_sensor();
		void destroy_sensor(sensor *handle);
	}
}

#endif // VIKKI_FILE_SYSTEM_USAGE_SENSOR_H
