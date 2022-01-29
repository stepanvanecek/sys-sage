set(CMAKE_INCLUDE_CURRENT_DIR ON)


set(SOURCES
    Topology.cpp
    DataPath.cpp
    CAT_aware.cpp
    parsers/hwloc.cpp
    parsers/caps-numa-benchmark.cpp)

set(HEADERS
    sys-topo.hpp
    Topology.hpp
    DataPath.hpp
    parsers/hwloc.hpp
    parsers/caps-numa-benchmark.hpp)

add_definitions(-DCAT_AWARE )

add_library(sys-topo STATIC ${SOURCES} ${HEADERS})

set_target_properties(sys-topo PROPERTIES
    PUBLIC_HEADER "sys-topo.hpp;parsers/hwloc.hpp;Topology.hpp"
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

target_include_directories(sys-topo PRIVATE .)

install(TARGETS sys-topo
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION inc
    ARCHIVE DESTINATION lib)

#.pc file
configure_file(sys-topo.pc.in sys-topo.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/src/sys-topo.pc DESTINATION ${CMAKE_INSTALL_PREFIX}/pkgconfig)
