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

#include "postgresql_storage.h"

#include <cstring>
#include <arpa/inet.h>

#include <iostream>
#include <thread>

namespace vikki
{
	postgresql_storage::postgresql_storage()
		: _conn(nullptr)
	{
	}

	postgresql_storage::~postgresql_storage()
	{
		close();
	}

	std::string postgresql_storage::name() const
	{
		return "postgresql";
	}

	void postgresql_storage::open(const std::map<std::string, std::string>& params)
	{
		if (_conn != nullptr)
		{
			close();
		}

		_conn_params = params;

		auto schema_iter = params.find("schema");
		_schema = schema_iter != params.end() ? schema_iter->second : "public";

		_conn = create_connection();
	}

	void postgresql_storage::close()
	{
		if (_conn == nullptr)
		{
			return;
		}

		PQfinish(_conn);
		_conn = nullptr;
	}

	void postgresql_storage::put_data(const std::string& sensor_name, std::time_t time, const std::vector<char>& data)
	{
		std::string query = "INSERT INTO " + _schema + "." + sensor_name + " ( ";
		query += "created, data ) VALUES ( to_timestamp($1), $2::bytea )";

		std::string time_str = std::to_string(time);

		const char *values[2];
		int lengths[2];
		int formats[2];

		values[0] = time_str.c_str();
		lengths[0] = 0;
		formats[0] = 0;

		values[1] = data.data();
		lengths[1] = data.size();
		formats[1] = 1;

		PGresult *result = PQexecParams(_conn, query.c_str(), 2, NULL, values, lengths, formats, 1);
		if (PQresultStatus(result) != PGRES_COMMAND_OK)
		{
			std::string error = "Can't put data of sensor " + sensor_name + ": ";
			error += PQerrorMessage(_conn);

			PQclear(result);

			throw exception(error);
		}

		PQclear(result);
	}

	storage::sensor_data_t postgresql_storage::get_data(const std::string& sensor_name, std::time_t from, std::time_t to)
	{
		PGconn *conn = create_connection();

		std::string query = "SELECT extract(epoch FROM t.created)::bigint AS time, t.data ";
		query += "FROM " + _schema + "." + sensor_name + " t ";
		query += "WHERE t.created BETWEEN to_timestamp($1) AND to_timestamp($2) ";
		query += "ORDER BY t.created ASC;";

		std::string from_str = std::to_string(from);
		std::string to_str = std::to_string(to);

		const char *values[2];
		int lengths[2];
		int formats[2];

		values[0] = from_str.c_str();
		lengths[0] = 0;
		formats[0] = 0;

		values[1] = to_str.c_str();
		lengths[1] = 0;
		formats[1] = 0;

		PGresult *result = PQexecParams(conn, query.c_str(), 2, NULL, values, lengths, formats, 1);
		if (PQresultStatus(result) != PGRES_TUPLES_OK)
		{
			std::string error = "Can't get data of sensor " + sensor_name + ": ";
			error += PQerrorMessage(conn);

			PQclear(result);

			throw exception(error);
		}

		sensor_data_t data;

		for (int i = 0; i < PQntuples(result); ++i)
		{
			std::time_t time = htonll(*reinterpret_cast<uint64_t*>(PQgetvalue(result, i, 0)));

			data[time] = std::vector<char>(PQgetlength(result, i, 1));

			std::memcpy(data[time].data(), PQgetvalue(result, i, 1), data[time].size());
		}

		PQclear(result);

		return data;
	}

	void postgresql_storage::prepare_entity(const std::string& sensor_name)
	{
		if (_conn == nullptr || PQstatus(_conn) != CONNECTION_OK)
		{
			throw exception("Can't prepare entity for sensor " + sensor_name + ": connection closed");
		}

		if (!is_entity_exists(sensor_name))
		{
			create_entity(sensor_name);
		}
	}

	PGconn* postgresql_storage::create_connection() const
	{
		std::string conn_info;

		auto param_iter = _conn_params.find("host");
		conn_info = std::string("host=") + (param_iter != _conn_params.end() ? param_iter->second : "localhost");

		param_iter = _conn_params.find("port");
		conn_info += std::string(" port=") + (param_iter != _conn_params.end() ? param_iter->second : "5432");

		param_iter = _conn_params.find("dbname");
		conn_info += std::string(" dbname=") + (param_iter != _conn_params.end() ? param_iter->second : "postgres");

		param_iter = _conn_params.find("user");
		conn_info += std::string(" user=") + (param_iter != _conn_params.end() ? param_iter->second : "postgres");

		param_iter = _conn_params.find("password");
		conn_info += std::string(" password=") + (param_iter != _conn_params.end() ? param_iter->second : "");

		PGconn *conn = PQconnectdb(conn_info.c_str());
		if (PQstatus(conn) != CONNECTION_OK)
		{
			std::string error = std::string("Can't connect to database: ") + PQerrorMessage(conn);

			PQfinish(conn);

			throw exception(error);
		}

		return conn;
	}

	bool postgresql_storage::is_entity_exists(const std::string& name)
	{
		std::string query = "SELECT count ( * ) > 0 ";
		query += "FROM pg_class cl ";
		query += "INNER JOIN pg_namespace ns ON ns.oid = cl.relnamespace ";
		query += "WHERE ns.nspname = $$" + _schema + "$$ AND ";
		query += "	cl.relname = $$" + name + "$$;";

		PGresult *result = PQexec(_conn, query.c_str());
		if (PQresultStatus(result) != PGRES_TUPLES_OK)
		{
			std::string error = "Can't check existing of entity " + name + ": ";
			error += PQerrorMessage(_conn);

			PQclear(result);

			throw exception(error);
		}

		bool exists = std::strcmp(PQgetvalue(result, 0, 0), "t") == 0;

		PQclear(result);

		return exists;
	}

	void postgresql_storage::create_entity(const std::string& name)
	{
		std::string query = "CREATE TABLE " + _schema + "." + name;
		query += " ( id serial NOT NULL, ";
		query += "created timestamptz NOT NULL, ";
		query += "data bytea NOT NULL );";

		PGresult *result = PQexec(_conn, query.c_str());
		if (PQresultStatus(result) != PGRES_COMMAND_OK)
		{
			std::string error = "Can't create entity " + name + ": ";
			error += PQerrorMessage(_conn);

			PQclear(result);

			throw exception(error);
		}

		PQclear(result);
	}

	uint64_t postgresql_storage::htonll(uint64_t value)
	{
		static const int num = 42;

		if (*reinterpret_cast<const char*>(&num) == num)
		{
			const uint32_t high_part = htonl(static_cast<uint32_t>(value >> 32));
			const uint32_t low_part = htonl(static_cast<uint32_t>(value & 0xFFFFFFFFLL));

			return (static_cast<uint64_t>(low_part) << 32) | high_part;
		}

		return value;
	}

	storage* create_storage()
	{
		return new postgresql_storage();
	}

	void destroy_storage(storage *handle)
	{
		delete handle;
	}
}
