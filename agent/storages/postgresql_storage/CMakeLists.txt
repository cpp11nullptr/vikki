cmake_minimum_required(VERSION 2.8)
project(postgresql_storage)
include_directories(
    include
    ../../core/include)
aux_source_directory(../../core/src SOURCES_BASE)
aux_source_directory(src SOURCES)
add_library(postgresql_storage SHARED ${SOURCES_BASE} ${SOURCES})
set(CMAKE_CXX_FLAGS "-std=c++11")
find_package(PostgreSQL REQUIRED)
include_directories(${PostgreSQL_INCLUDE_DIRS})
target_link_libraries(postgresql_storage ${PostgreSQL_LIBRARIES})
install(TARGETS postgresql_storage DESTINATION bin/vikki/storages)
