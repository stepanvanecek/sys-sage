

link_libraries(sys-topo sys-topo)
include_directories(../src)

add_executable(example1 main.cpp)
install(TARGETS example1 DESTINATION bin)
