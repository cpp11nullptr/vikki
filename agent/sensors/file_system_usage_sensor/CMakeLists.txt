cmake_minimum_required(VERSION 2.8)
project(file_system_usage_sensor)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(
    include
    ../../core/include)
aux_source_directory(../../core/src SOURCES_BASE)
aux_source_directory(src SOURCES)
add_library(file_system_usage_sensor SHARED ${SOURCES_BASE} ${SOURCES})
install(TARGETS file_system_usage_sensor DESTINATION bin/vikki/sensors)
