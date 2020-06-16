include(ExternalProject)

cmake_minimum_required(VERSION 3.17)

project(mystorecpp VERSION 1.0)
set(CMAKE_CXX_FLAGS "-Wall -pedantic -g -O3")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(ClientTestFiles src/mystorecpp.cpp tests/testclient.cpp)
set(ClientFiles src/mystorecpp.cpp)

ExternalProject_Add(cpp-dotenv
    PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/cpp-dotenv
    GIT_REPOSITORY https://github.com/adeharo9/cpp-dotenv.git
    INSTALL_COMMAND ""
)

ExternalProject_Add(thrift-file
    PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/thriftfile
    GIT_REPOSITORY https://github.com/gwint/mystore-thrift-file.git
    INSTALL_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp
           ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/ReplicaService.cpp
           ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/replicaservice_types.cpp
           ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/replicaservice_constants.cpp
    COMMAND thrift -r --gen cpp ${CMAKE_CURRENT_SOURCE_DIR}/thriftfile/src/thrift-file/replicaservice.thrift
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/thriftfile/src/thrift-file/replicaservice.thrift
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(ThriftFileCompilation
    ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/ReplicaService.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/replicaservice_types.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gen-cpp/replicaservice_constants.cpp
)

add_library(MyStoreClient ${ClientFiles})

target_include_directories(MyStoreClient
    PUBLIC cpp-dotenv/src/cpp-dotenv/
    PUBLIC gen-cpp
    PUBLIC include
)

add_executable(MyStoreTests ${ClientTestFiles})

target_include_directories(MyStoreTests
    PUBLIC cpp-dotenv/src/cpp-dotenv/
    PUBLIC gen-cpp
    PUBLIC include
)

add_dependencies(ThriftFileCompilation thrift-file)

add_dependencies(MyStoreClient cpp-dotenv)
add_dependencies(MyStoreClient thrift-file)
target_link_libraries(MyStoreClient ThriftFileCompilation)
target_link_libraries(MyStoreClient thrift)

add_dependencies(MyStoreTests cpp-dotenv)
add_dependencies(MyStoreTests thrift-file)
target_link_libraries(MyStoreTests ThriftFileCompilation)
target_link_libraries(MyStoreTests thrift)
