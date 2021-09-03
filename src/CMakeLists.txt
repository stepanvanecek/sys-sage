set(CMAKE_INCLUDE_CURRENT_DIR ON)


set(CMAKE_CXX_FLAGS "-Wall -std=c++11")


#$(shell xml2-config --cflags)
#$(shell xml2-config --libs)
include_directories(${LIBXML2_INCLUDE_DIRS})
link_libraries(${LIBXML2_LIBRARY})

set(SOURCES
    hwloc.cpp
    Topology.cpp)

set(HEADERS
    hwloc.hpp
    Topology.hpp)

add_library(sys-topo STATIC ${SOURCES} ${HEADERS})

set_target_properties(sys-topo PROPERTIES
    PUBLIC_HEADER "hwloc.hpp;Topology.hpp"
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

target_include_directories(sys-topo PRIVATE .)

install(TARGETS sys-topo
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

#.pc file
configure_file(sys-topo.pc.in sys-topo.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/src/sys-topo.pc DESTINATION ${CMAKE_INSTALL_PREFIX}/pkgconfig)
