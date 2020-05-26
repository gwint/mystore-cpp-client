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

#set(ClientFiles lib/mystorecpp.cpp tests/testclient.cpp)
set(ClientTestFiles lib/mystorecpp.cpp tests/testclient.cpp)
set(ClientFiles lib/mystorecpp.cpp)

add_library(MyStoreClient ${ThriftGeneratedFiles} ${ClientFiles})

target_include_directories(MyStoreClient
    PUBLIC cpp-dotenv/src/cpp-dotenv/
    PUBLIC gen-cpp
    PUBLIC include
)

add_dependencies(MyStoreClient cpp-dotenv)

add_executable(MyStoreTests ${ThriftGeneratedFiles} ${ClientTestFiles})

target_include_directories(MyStoreTests
    PUBLIC cpp-dotenv/src/cpp-dotenv/
    PUBLIC gen-cpp
    PUBLIC include
)

add_dependencies(MyStoreTests cpp-dotenv)

target_link_libraries(MyStoreClient thrift)
target_link_libraries(MyStoreTests thrift)
