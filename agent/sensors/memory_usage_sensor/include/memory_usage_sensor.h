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

#ifndef VIKKI_MEMORY_USAGE_SENSOR_H
#define VIKKI_MEMORY_USAGE_SENSOR_H

#include "exception.h"
#include "sensor.h"

#include <string>

namespace vikki
{
	class memory_usage_sensor
		: public sensor
	{
	public:
		memory_usage_sensor();
		~memory_usage_sensor() override;

		std::string name() const override;
		std::vector<char> data() override;

	private:
		uint64_t parse_mem_param(char *buffer, const char *attr) const;

	};

	extern "C"
	{
		sensor* create_sensor();
		void destroy_sensor(sensor *handle);
	}
}

#endif // VIKKI_MEMORY_USAGE_SENSOR_H
