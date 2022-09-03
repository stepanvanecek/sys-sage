set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SOURCES
    Topology.cpp
    DataPath.cpp
    CAT_aware.cpp
    xml_dump.cpp
    parsers/hwloc.cpp
    parsers/caps-numa-benchmark.cpp)

set(HEADERS
    sys-sage.hpp
    Topology.hpp
    DataPath.hpp
    xml_dump.hpp
    parsers/hwloc.hpp
    parsers/caps-numa-benchmark.hpp)

add_library(sys-sage STATIC ${SOURCES} ${HEADERS})

set_target_properties(sys-sage PROPERTIES
    PUBLIC_HEADER "sys-sage.hpp;Topology.hpp;DataPath.hpp;xml_dump.hpp;parsers/hwloc.hpp;parsers/caps-numa-benchmark.hpp"
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
)

target_include_directories(sys-sage PRIVATE .)

install(DIRECTORY "."
    DESTINATION inc
    FILES_MATCHING PATTERN "*.hpp")
install(TARGETS sys-sage
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION inc
    ARCHIVE DESTINATION lib)
install(TARGETS sys-sage
        EXPORT sys-sage
        DESTINATION lib
)
install(EXPORT sys-sage
        DESTINATION lib
)
#export(TARGETS sys-sage
#        FILE sys-sage-targets.cmake
#)
