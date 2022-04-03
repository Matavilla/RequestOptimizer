cmake_minimum_required(VERSION 3.8)

project(RequestOptimizer)

option(DEBUG "Enable debug" OFF)

set(CXX_FLAGS_COMMON "-Wall -std=c++2a")
set(CXX_FLAGS_DEBUG "${CXX_FLAGS_COMMON} -g")
set(CXX_FLAGS_RELEASE "${CXX_FLAGS_COMMON} -O3")

if (DEBUG) 
    set(CMAKE_CXX_FLAGS "${CXX_FLAGS_DEBUG}")
else()
    set(CMAKE_CXX_FLAGS "${CXX_FLAGS_RELEASE}")
endif()

#add_library(Routing RoutAlgo/RoutingDijkstra.h RoutAlgo/RoutingDijkstra.cpp)
#add_library(Network Network/Network.h Network/Network.cpp)
#add_library(Scheduler Scheduler/Scheduler.h Scheduler/Scheduler.cpp)
#add_library(XmlLibrary XmlLibrary/tinyxml2.cpp XmlLibrary/tinyxml2.h)

include_directories(VM)
include_directories(Task)
include_directories(Solver)
include_directories(XmlLibrary)

set(CPP_SOURCES
    Task/Task.cpp
    Task/Work.cpp
    Solver/Solver.cpp
    XmlLibrary/tinyxml2.cpp
    main.cpp)

set(CPP_HEADERS
    VM/VM.h
    VM/VMParam.h
    Task/Task.h
    Task/Work.h
    XmlLibrary/tinyxml2.h
    Solver/Solver.h)

add_executable(${PROJECT_NAME} ${CPP_HEADERS} ${CPP_SOURCES})
#target_link_libraries(${PROJECT_NAME} Network)
#target_link_libraries(${PROJECT_NAME} Routing)
#target_link_libraries(${PROJECT_NAME} Scheduler)
#target_link_libraries(${PROJECT_NAME} XmlLibrary)
