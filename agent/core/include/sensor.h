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

#ifndef VIKKI_AGENT_SENSOR_H
#define VIKKI_AGENT_SENSOR_H

#include <string>
#include <vector>
#include <map>

namespace vikki
{
	class sensor
	{
	public:
		sensor();
		virtual ~sensor();

		virtual std::string name() const = 0;
		virtual std::vector<char> data() = 0;

		virtual void init(const std::map<std::string, std::string>& params);

	protected:
		template<typename T>
		void write_data(void *p, T value, void **pp);

	};

	template<typename T>
	void sensor::write_data(void *p, T value, void **pp)
	{
		T *pT = reinterpret_cast<T*>(p);

		*pT = value;
		*pp = pT + 1;
	}
}

#endif // VIKKI_AGENT_SENSOR_H

