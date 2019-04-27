#
# FindDrogon submodule and setup monero cmake target 
#
#
# by moneroexamples

message(STATUS "Searching for Drogon")

add_library(Drogon::Drogon INTERFACE IMPORTED GLOBAL)
find_package (Jsoncpp REQUIRED)

set_target_properties(Drogon::Drogon PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES 
      "${CMAKE_CURRENT_SOURCE_DIR}/ext/drogon/lib/inc;${CMAKE_CURRENT_SOURCE_DIR}/ext/drogon/lib/src;${CMAKE_CURRENT_SOURCE_DIR}/ext/drogon/orm_lib/inc;${CMAKE_CURRENT_SOURCE_DIR}/ext/drogon/trantor;${JSONCPP_INCLUDE_DIRS}")

target_link_libraries(Drogon::Drogon INTERFACE
    drogon trantor)

message(STATUS "Drogon::Drogon interface target defined")
