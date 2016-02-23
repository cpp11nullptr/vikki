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

#include "file_system_usage_sensor.h"

#include <cstdlib>
#include <cerrno>
#include <mntent.h>
#include <cstring>
#include <iostream>

#if defined(__FreeBSD__) || defined(__OpenBSD__)
# include <sys/param.h>
# include <sys/mount.h>
#else
# include <sys/statvfs.h>
# define VIKKI_STATVFS
#endif

namespace vikki
{
	file_system_usage_sensor::file_system_usage_sensor()
	{
	}

	file_system_usage_sensor::~file_system_usage_sensor()
	{
	}

	std::string file_system_usage_sensor::name() const
	{
		return "file_system_usage";
	}

	std::vector<char> file_system_usage_sensor::data()
	{
		uint64_t size = 0;

		std::vector<file_system> systems = get_file_system_list();
		size += sizeof(uint64_t);

		for (const file_system& system : systems)
		{
			size += sizeof(uint64_t);
			size += system.dir.size();

			size += sizeof(uint64_t);
			size += system.device.size();

			size += sizeof(uint64_t);
			size += system.type.size();

			size += sizeof(uint64_t);
			size += system.options.size();

			size += sizeof(uint64_t) * 5;
		}

		std::vector<char> result(size);
		char *p = result.data();

		*reinterpret_cast<uint64_t*>(p) = systems.size();
		p += sizeof(uint64_t);

		for (const file_system& system : systems)
		{
			*reinterpret_cast<uint64_t*>(p) = system.dir.size();
			p += sizeof(uint64_t);

			std::memcpy(p, system.dir.c_str(), system.dir.size());
			p += system.dir.size();

			//

			*reinterpret_cast<uint64_t*>(p) = system.device.size();
			p += sizeof(uint64_t);

			std::memcpy(p, system.device.c_str(), system.device.size());
			p += system.device.size();

			//

			*reinterpret_cast<uint64_t*>(p) = system.type.size();
			p += sizeof(uint64_t);

			std::memcpy(p, system.type.c_str(), system.type.size());
			p += system.type.size();

			//

			*reinterpret_cast<uint64_t*>(p) = system.options.size();
			p += sizeof(uint64_t);

			std::memcpy(p, system.options.c_str(), system.options.size());
			p += system.options.size();

			//

			file_system_info info = get_file_system_info(system.dir);

			*reinterpret_cast<uint64_t*>(p) = info.total;
			p += sizeof(uint64_t);

			*reinterpret_cast<uint64_t*>(p) = info.free;
			p += sizeof(uint64_t);

			*reinterpret_cast<uint64_t*>(p) = info.avail;
			p += sizeof(uint64_t);

			*reinterpret_cast<uint64_t*>(p) = info.files;
			p += sizeof(uint64_t);

			*reinterpret_cast<uint64_t*>(p) = info.free_files;
			p += sizeof(uint64_t);
		}

		return result;
	}

	uint64_t file_system_usage_sensor::blocks_to_bytes(uint64_t value, uint64_t bsize) const
	{
		return (value * bsize) >> 1;
	}

	std::vector<file_system_usage_sensor::file_system> file_system_usage_sensor::get_file_system_list() const
	{
		std::vector<file_system> systems;

		FILE *fp = setmntent(MOUNTED, "r");
		if (!fp)
		{
			throw exception("Can't open mounted file system list, error code: " + std::to_string(errno));
		}

		mntent data;
		char buffer[1025];

		while (getmntent_r(fp, &data, buffer, sizeof(buffer)))
		{
			file_system system;

			system.dir = data.mnt_dir;
			system.device = data.mnt_fsname;
			system.type = data.mnt_type;
			system.options = data.mnt_opts;

			if (system.options.find("nodev") != std::string::npos)
			{
				continue;
			}

			if (system.type == "devtmpfs" || system.type == "devpts" || system.type == "rootfs")
			{
				continue;
			}

			systems.push_back(system);
		}

		endmntent(fp);

		return systems;
	}

	file_system_usage_sensor::file_system_info file_system_usage_sensor::get_file_system_info(const std::string& dir) const
	{
		file_system_info info;

#ifdef VIKKI_STATVFS
		struct statvfs data;
		int status = statvfs(dir.c_str(), &data);
#else
		struct statfs buf;
		int status = statfs(dir.c_str(), &data);
#endif

		if (status != 0)
		{
			throw exception("Can't get information about the filesystems, error code: " + std::to_string(errno));
		}

		uint64_t value, bsize;

#ifdef VIKKI_STATVFS_EXISTS
		bsize = data.f_frsize / 512;
#else
		bsize = data.f_bsize / 512;
#endif

		value = data.f_blocks;
		info.total = blocks_to_bytes(value, bsize);

		value = data.f_bfree;
		info.free  = blocks_to_bytes(value, bsize);

		value = data.f_bavail;
		info.avail = blocks_to_bytes(value, bsize);

		info.files = data.f_files;
		info.free_files = data.f_ffree;

		return info;
	}

	sensor *create_sensor()
	{
		return new file_system_usage_sensor();
	}

	void destroy_sensor(sensor *handle)
	{
		delete handle;
	}
}
