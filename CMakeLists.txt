
cmake_policy(SET CMP0048 NEW)

project(sys-topo VERSION 0.0.0 LANGUAGES C CXX )

cmake_minimum_required(VERSION 3.10.0)
set (CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS " -Wall -O0")


find_package(LibXml2 REQUIRED) # Use pkg-config via the LibXml2 find-module

#$(shell xml2-config --cflags)
#$(shell xml2-config --libs)
include_directories(${LIBXML2_INCLUDE_DIRS})
link_libraries(${LIBXML2_LIBRARY})

# Top-level build just includes subdirectories.
add_subdirectory(src)
add_subdirectory(examples)
