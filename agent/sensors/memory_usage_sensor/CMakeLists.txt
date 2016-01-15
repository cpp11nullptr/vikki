cmake_minimum_required(VERSION 2.8)
project(memory_usage_sensor)
include_directories(
    include
    ../../core/include)
aux_source_directory(../../core/src SOURCES_BASE)
aux_source_directory(src SOURCES)
add_library(memory_usage_sensor SHARED ${SOURCES_BASE} ${SOURCES})
set(CMAKE_CXX_FLAGS "-std=c++11")
install(TARGETS memory_usage_sensor DESTINATION bin/vikki/sensors)
