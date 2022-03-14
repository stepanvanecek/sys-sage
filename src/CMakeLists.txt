set(CMAKE_INCLUDE_CURRENT_DIR ON)


set(SOURCES
    Topology.cpp
    DataPath.cpp
    CAT_aware.cpp
    parsers/hwloc.cpp
    parsers/caps-numa-benchmark.cpp)

set(HEADERS
    sys-sage.hpp
    Topology.hpp
    DataPath.hpp
    parsers/hwloc.hpp
    parsers/caps-numa-benchmark.hpp)

add_library(sys-sage STATIC ${SOURCES} ${HEADERS})

set_target_properties(sys-sage PROPERTIES
    PUBLIC_HEADER "sys-sage.hpp;Topology.hpp;DataPath.hpp;parsers/hwloc.hpp;parsers/caps-numa-benchmark.hpp"
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

target_include_directories(sys-sage PRIVATE .)

install(TARGETS sys-sage
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION inc
    ARCHIVE DESTINATION lib)

#.pc file
configure_file(sys-sage.pc.in sys-sage.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/src/sys-sage.pc DESTINATION ${CMAKE_INSTALL_PREFIX}/pkgconfig)
