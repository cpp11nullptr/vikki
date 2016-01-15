cmake_minimum_required(VERSION 2.8)
project(vikki_agent_core)
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
aux_source_directory(src SOURCES_BASE)
aux_source_directory(src/network SOURCES_NETWORK)
add_executable(vikki-agent ${SOURCES_BASE} ${SOURCES_NETWORK})
set(CMAKE_CXX_FLAGS "-pthread -std=c++11")
target_link_libraries(vikki-agent dl pthread ssl crypto)
install(TARGETS vikki-agent DESTINATION bin/vikki)
