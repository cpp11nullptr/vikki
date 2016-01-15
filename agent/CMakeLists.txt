cmake_minimum_required(VERSION 2.8)
project(vikki_agent)

option(VIKKI_POSTGRESQL_STORAGE "PostgreSQL storage" TRUE)

option(VIKKI_LOAD_AVERAGE_SENSOR "Load average sensor" TRUE)
option(VIKKI_MEMORY_USAGE_SENSOR "Memory usage sensor" TRUE)

add_subdirectory(core)
add_subdirectory(storages)
add_subdirectory(sensors)

install(FILES vikki-agent.cfg DESTINATION bin/vikki)
