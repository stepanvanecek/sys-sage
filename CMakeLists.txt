
cmake_policy(SET CMP0048 NEW)

project(sys-topo VERSION 0.0.0 LANGUAGES C CXX )

cmake_minimum_required(VERSION 3.21.0)
set (CMAKE_CXX_STANDARD 11)

find_package(LibXml2 REQUIRED) # Use pkg-config via the LibXml2 find-module

# Top-level build just includes subdirectories.
add_subdirectory(src)
add_subdirectory(examples)
