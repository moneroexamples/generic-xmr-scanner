#
# FindDrogon submodule and setup monero cmake target 
#
#
# by moneroexamples



message(STATUS "Searching for Drogon")


# we are going to search for these libraries
#set(LIBS trantor drogon)



#set(DROGON_LIBRARIES "")

#foreach (l ${LIBS})

    #message(STATUS "Searching for ${l} library")
	
    #string(TOUPPER ${l} L)
	
    ##PATHS ${CMAKE_CURRENT_SOURCE_DIR}/ext/

    #find_library(${L}_LIBRARY
			#NAMES ${l}
            #PATHS ${CMAKE_CURRENT_BINARY_DIR}/ext/
            #PATH_SUFFIXES drogon drogon/trantor
            #NO_DEFAULT_PATH)

    #message(STATUS "Found ${${L}_LIBRARY}")

    ##add_library(${l} STATIC IMPORTED)
    ##set_property(TARGET ${l} PROPERTY IMPORTED_LOCATION ${${L}_LIBRARIES})

#endforeach()

add_library(Drogon::Drogon INTERFACE IMPORTED GLOBAL)

set_target_properties(Drogon::Drogon PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES 
      "${CMAKE_CURRENT_SOURCE_DIR}/ext/drogon/lib/inc;${CMAKE_CURRENT_SOURCE_DIR}/ext/drogon/lib/src;${CMAKE_CURRENT_SOURCE_DIR}/ext/drogon/orm_lib/inc;${CMAKE_CURRENT_SOURCE_DIR}/ext/drogon/trantor;")

target_link_libraries(Drogon::Drogon INTERFACE
    drogon trantor)

message(STATUS "Drogon::Drogon interface target defined")
