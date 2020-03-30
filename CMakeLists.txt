cmake_minimum_required(VERSION 3.15...3.17)
project(agoNetwork)

# include whole files to the project
include_directories(${CMAKE_SOURCE_DIR})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-fconcepts")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

#------------------------------------------------------------------------------------
# zmq library
#
## load in pkg-config support
find_package(PkgConfig)
## use pkg-config to get hints for 0mq locations
pkg_check_modules(PC_ZeroMQ QUIET zmq)

## use the hint from above to find where 'zmq.hpp' is located
find_path(ZeroMQ_INCLUDE_DIR
        NAMES zmq.hpp
        PATHS ${PC_ZeroMQ_INCLUDE_DIRS}
        )

## use the hint from about to find the location of libzmq
find_library(ZeroMQ_LIBRARY
        NAMES zmq
        PATHS ${PC_ZeroMQ_LIBRARY_DIRS}
        )
#------------------------------------------------------------------------------------

# AGO Network Library
add_library(agoNetwork SHARED)
target_sources(agoNetwork
        PRIVATE
        lib/network/zmq/zmqContext.cpp
        lib/network/router/router.cpp
        lib/network/dealer/dealer.cpp
        lib/network/socket/socket.cpp
        lib/network/zmq/zmqContext.h
        lib/network/zmq/zhelpers.hpp
        PUBLIC
        lib/concepts/concepts.h
        lib/network/router/router.h
        lib/network/dealer/dealer.h
        lib/network/socket/socket.h
        )

#------------------------------------------------------------------------------------
# link zmq library
#
## add the include directory to our compile directives
target_include_directories(agoNetwork PUBLIC ${ZeroMQ_INCLUDE_DIR})
## at the 0mq library to our link directive
target_link_libraries(agoNetwork PUBLIC ${ZeroMQ_LIBRARY} pthread)
#------------------------------------------------------------------------------------

# Unit Test
#add_executable(unitTest unitTest.cpp)
#target_link_libraries(unitTest agoNetwork)