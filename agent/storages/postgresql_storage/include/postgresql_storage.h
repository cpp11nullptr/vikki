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

#ifndef VIKKI_POSTGRESQL_STORAGE_H
#define VIKKI_POSTGRESQL_STORAGE_H

#include "exception.h"
#include "storage.h"

#include <libpq-fe.h>

namespace vikki
{
	class postgresql_storage
		: public storage
	{
	public:
		postgresql_storage();
		~postgresql_storage() override;

		std::string name() const override;

		void open(const std::map<std::string, std::string>& params) override;
		void close() override;

		void put_data(const std::string& sensor_name, std::time_t time, const std::vector<char>& data) override;
		sensor_data_t get_data(const std::string& sensor_name, std::time_t from, std::time_t to) override;

		void prepare_entity(const std::string& sensor_name) override;

	private:
		std::map<std::string, std::string> _conn_params;
		PGconn *_conn;
		std::string _schema;

		PGconn* create_connection() const;

		bool is_entity_exists(const std::string& name);
		void create_entity(const std::string& name);

		static uint64_t htonll(uint64_t value);

	};

	extern "C"
	{
		storage* create_storage();
		void destroy_storage(storage *handle);
	}
}

#endif // VIKKI_POSTGRESQL_STORAGE_H

