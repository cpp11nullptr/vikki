cmake_minimum_required(VERSION 3.1)
project(memory_usage_sensor)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_SYSTEM_NAME STREQUAL FreeBSD)
	include_directories("/usr/local/include")
	link_directories("/usr/local/lib")
endif()

include_directories(
    include
    ../../core/include)

aux_source_directory(../../core/src SOURCES_BASE)
aux_source_directory(src SOURCES)
add_library(memory_usage_sensor SHARED ${SOURCES_BASE} ${SOURCES})
install(TARGETS memory_usage_sensor DESTINATION bin/vikki/sensors)
