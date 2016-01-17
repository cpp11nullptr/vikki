cmake_minimum_required(VERSION 3.1)
project(postgresql_storage)

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

add_library(postgresql_storage SHARED ${SOURCES_BASE} ${SOURCES})
find_package(PostgreSQL REQUIRED)
include_directories(${PostgreSQL_INCLUDE_DIRS})
target_link_libraries(postgresql_storage ${PostgreSQL_LIBRARIES})
install(TARGETS postgresql_storage DESTINATION bin/vikki/storages)
