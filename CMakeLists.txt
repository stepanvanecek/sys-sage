macro(print_all_variables)
    message(STATUS "print_all_variables------------------------------------------{")
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
    message(STATUS "print_all_variables------------------------------------------}")
endmacro()


cmake_policy(SET CMP0048 NEW)
project(sys-sage VERSION 0.0.0 LANGUAGES C CXX )

cmake_minimum_required(VERSION 3.12.0)
#cmake_minimum_required(VERSION 3.10.0)
set (CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS " -Wall -O0")

find_package(LibXml2 REQUIRED) # Use pkg-config via the LibXml2 find-module

execute_process(COMMAND xml2-config --cflags OUTPUT_VARIABLE LIBXML_F OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND xml2-config --libs OUTPUT_VARIABLE LIBXML_L OUTPUT_STRIP_TRAILING_WHITESPACE)
include_directories(${LIBXML_F})
link_libraries(${LIBXML_L})

include_directories(${LIBXML2_INCLUDE_DIR})
include_directories(${LIBXML2_INCLUDE_DIRS})
link_libraries(${LIBXML2_LIBRARY})
link_libraries(${LIBXML2_LIBRARIES})
#print_all_variables()

###Options:
#add_definitions(-DCAT_AWARE )
option(CAT_AWARE "Build and install functionality regarding Intel L3 CAT" OFF)
option(BUILD_DATA_SOURCES "Build and install contents of data-sources folder" OFF)

if(CAT_AWARE)
    add_definitions(-DCAT_AWARE )
endif()

# Top-level build just includes subdirectories.
add_subdirectory(src)
add_subdirectory(examples)

if(BUILD_DATA_SOURCES)
    add_subdirectory(data-sources)
endif()
