cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME
        xmrscanner)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

list(APPEND CMAKE_MODULE_PATH 
    "${CMAKE_CURRENT_LIST_DIR}/ext/xmregcore/cmake"
    "${CMAKE_CURRENT_LIST_DIR}/cmake")

# add XMREGCORE submodule
set(BUILD_XMREGCORE_TESTS OFF CACHE INTERNAL "")
add_subdirectory(ext/xmregcore)

set(BUILD_ORM FALSE CACHE INTERNAL "")
set(BUILD_CTL FALSE CACHE INTERNAL "")
set(BUILD_EXAMPLES FALSE CACHE INTERNAL "")
add_subdirectory(ext/drogon)

# this find will just give as interface target
# Drogon::Drogon
# it does not really checks for drogon libraries presence
# it assumes they exists
find_package(Drogon)

find_package(Boost REQUIRED
    COMPONENTS fiber)

add_executable(${PROJECT_NAME}
        main.cpp
        src/utils.cpp
        src/controllers/SearchCtrl.cpp
        src/controllers/InfoCtrl.cpp
        src/config/IConfigReader.cpp
        src/config/DefaultAddresses.cpp
        src/config/DefaultConfig.cpp
        src/searchtasks/SearchTask.cpp
        src/searchtasks/OutputSearchTask.cpp
        src/searchtasks/InputSearchTask.cpp
        src/searchtasks/InputSearchLiteTask.cpp)

target_include_directories(${PROJECT_NAME}
    PRIVATE 
        ext/fiberpool/include
        ext/)

target_link_libraries(${PROJECT_NAME}
    PRIVATE 
        XMREG::core 
        Drogon::Drogon  
        Boost::fiber
        stdc++fs)

# copy scanners confing files into build folder
configure_files(${CMAKE_CURRENT_SOURCE_DIR}/config
                ${CMAKE_CURRENT_BINARY_DIR}/config)

if (${BUILD_TESTS})
    enable_testing()
    add_subdirectory(ext/googletest)
    add_subdirectory(tests)
endif()

