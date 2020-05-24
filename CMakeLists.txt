include(ExternalProject)

cmake_minimum_required(VERSION 3.10)

project(mystorecpp VERSION 1.0)
set(CMAKE_CXX_FLAGS "-Wall -pedantic -g -O3")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

execute_process(
    COMMAND thrift -r --gen cpp replicaservice.thrift
    COMMAND thrift -r --gen py replicaservice.thrift
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/
)

ExternalProject_Add(cpp-dotenv
    PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/cpp-dotenv
    GIT_REPOSITORY https://github.com/adeharo9/cpp-dotenv.git
    INSTALL_COMMAND ""
)

set(ThriftGeneratedFiles
    ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/ReplicaService.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/replicaservice_types.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/replicaservice_constants.cpp
)

set(ClientFiles replica.cpp lockhandler.cpp replica.hpp lockhandler.hpp)

add_executable(MyStoreClient ${ThriftGeneratedFiles} ${ClientFiles})

add_dependencies(MyStoreClient cpp-dotenv)

target_include_directories(MyStoreClient PRIVATE cpp-dotenv/src/cpp-dotenv/)

target_link_libraries(MyStoreClient thrift)