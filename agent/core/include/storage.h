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

#ifndef VIKKI_AGENT_STORAGE_H
#define VIKKI_AGENT_STORAGE_H

#include <string>
#include <ctime>
#include <vector>
#include <map>

namespace vikki
{
	class storage
	{
	public:
		using sensor_data_t = std::map<std::time_t, std::vector<char>>;

		storage();
		virtual ~storage();

		virtual std::string name() const = 0;

		virtual void open(const std::map<std::string, std::string>& params) = 0;
		virtual void close() = 0;

		virtual void put_data(const std::string& sensor_name, std::time_t time, const std::vector<char>& data) = 0;
		virtual sensor_data_t get_data(const std::string& sensor_name, std::time_t from, std::time_t to) = 0;

		virtual void prepare_entity(const std::string& sensor_name) = 0;

	};
}

#endif // VIKKI_AGENT_STORAGE_H

