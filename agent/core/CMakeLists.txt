cmake_minimum_required(VERSION 2.8)
project(vikki_agent_core)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_SYSTEM_NAME STREQUAL FreeBSD)
	include_directories("/usr/local/include")
	link_directories("/usr/local/lib")
endif()

include_directories(
    include
    include/network
    include/asio
    include/asio/detail
    include/asio/impl
    include/asio/detail/impl
    include/asio/ip
    include/asio/ip/impl
    include/asio/ip/detail
    include/asio/generic/detail
    include/asio/local/detail)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
#find_library(DL dl REQUIRED)

aux_source_directory(src SOURCES_BASE)
aux_source_directory(src/network SOURCES_NETWORK)

add_executable(vikki-agent ${SOURCES_BASE} ${SOURCES_NETWORK})
target_link_libraries(vikki-agent ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES})
install(TARGETS vikki-agent DESTINATION bin/vikki)
