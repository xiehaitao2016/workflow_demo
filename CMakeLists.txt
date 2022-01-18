cmake_minimum_required(VERSION 3.15)
project(CSROS_PERIPHERAL LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)



set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(PROJECT_INCLUDE ${PROJECT_SOURCE_DIR}/include)
set(PROJECT_SRC ${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_INCLUDE})
include_directories(${PROJECT_SRC})
include_directories(thirdparty/workflow/_include)


set(SERVER
#    src/test.cpp
#    include/test.h
)

add_subdirectory(thirdparty/workflow)
#
add_executable(test.out main.cpp ${SERVER})
target_link_libraries(test.out
        PRIVATE
        workflow
        ssl
        crypto)
target_compile_definitions(test.out PRIVATE EC_VER2)


find_package(OpenSSL REQUIRED)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})
set(WORKFLOW_LIB workflow pthread OpenSSL::SSL OpenSSL::Crypto)
set(DIR10 user_defined_protocol)
add_executable(k2server ${DIR10}/k2server.cpp ${DIR10}/K2Message.cpp ${DIR10}/K2Message.h)
add_executable(server1 ${DIR10}/server.cc ${DIR10}/message.cc)
add_executable(client1 ${DIR10}/client.cc ${DIR10}/message.cc)
target_link_libraries(server1 ${WORKFLOW_LIB})
target_link_libraries(k2server ${WORKFLOW_LIB})
target_link_libraries(client1 ${WORKFLOW_LIB})

set_target_properties(k2server PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/${DIR10})
set_target_properties(server1 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/${DIR10})
set_target_properties(client1 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/${DIR10})