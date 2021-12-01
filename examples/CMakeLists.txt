

link_libraries(sys-topo sys-topo)
include_directories(../src)

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

add_library(numa)

add_executable(basic_usage basic_usage.cpp)
add_executable(bw bw.cpp)
target_link_libraries(bw -lnuma)
install(TARGETS basic_usage bw DESTINATION bin)
