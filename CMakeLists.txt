cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME
        xmrscanner)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

list(APPEND CMAKE_MODULE_PATH 
    "${CMAKE_CURRENT_LIST_DIR}/ext/xmregcore/cmake"
    "${CMAKE_CURRENT_LIST_DIR}/cmake")

#include(MyUtils)

# add XMREGCORE submodule
set(BUILD_XMREGCORE_TESTS OFF CACHE INTERNAL "")
add_subdirectory(ext/xmregcore)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(ext/nlohmann)

#set(BUILD_CTL FALSE CACHE INTERNAL "")
#set(BUILD_ORM FALSE CACHE INTERNAL "")
add_subdirectory(ext/drogon)

# this find will just give as interface target
# Drogon::Drogon
find_package(Drogon)


find_package(Boost REQUIRED
    COMPONENTS fiber)

add_executable(${PROJECT_NAME}
        main.cpp
        src/utils.cpp
        src/SearchCtrl.cpp)

target_include_directories(${PROJECT_NAME}
    PRIVATE ext/fiberpool/include)

target_link_libraries(${PROJECT_NAME}
    PRIVATE 
        XMREG::core 
        Drogon::Drogon  
        nlohmann_json::nlohmann_json
        Boost::fiber
        stdc++fs)
        

if (${BUILD_TESTS})
    include_directories(
        ${CMAKE_SOURCE_DIR}/ext/googletest/googletest/include
        ${CMAKE_SOURCE_DIR}/ext/googletest/googlemock/include)
endif()

if (${BUILD_TESTS})
    enable_testing()
    add_subdirectory(ext/googletest)
    add_subdirectory(tests)
endif()

