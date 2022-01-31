

link_libraries(sys-sage sys-sage)
include_directories(../src)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

add_library(numa bw)

add_executable(basic_usage basic_usage.cpp)
add_executable(bw bw.cpp)
add_executable(matmul matmul.cpp)
set_source_files_properties(matmul.cpp PROPERTIES COMPILE_FLAGS -O3)


target_link_libraries(basic_usage hwloc )
target_link_libraries(matmul hwloc pqos )
target_link_libraries(bw -lnuma)
install(TARGETS basic_usage bw matmul DESTINATION bin)
