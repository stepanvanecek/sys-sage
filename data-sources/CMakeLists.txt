

###### hwloc
if(DS_HWLOC)
    message(STATUS "DS_HWLOC - hwloc data source")
    find_package(OpenMP)
    if (OPENMP_FOUND)
        set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    endif()

    add_executable(hwloc-output hwloc-output.cpp)
    target_link_libraries(hwloc-output hwloc)
    install(TARGETS hwloc-output DESTINATION bin)
endif()

###### numa
if(DS_NUMA)
    message(STATUS "DS_NUMA - cape-numa-benchmark data source")
    add_executable(caps-numa-benchmark caps-numa-benchmark.cpp)
    target_link_libraries(caps-numa-benchmark numa)
    install(TARGETS caps-numa-benchmark DESTINATION bin)
endif()

###### mt4g
if(DS_MT4G)
    message(STATUS "DS_MT4G - mt4g data source")
    find_package(Git QUIET)
    if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
        # Update submodules as needed
        option(GIT_SUBMODULE "Check submodules during build" ON)
        if(GIT_SUBMODULE)
            message(STATUS "Submodule update")
            execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                            RESULT_VARIABLE GIT_SUBMOD_RESULT)
            if(NOT GIT_SUBMOD_RESULT EQUAL "0")
                message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
            endif()
        endif()
    endif()
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/mt4g/CMakeLists.txt")
        message(STATUS "Path ${PROJECT_SOURCE_DIR}/mt4g/CMakeLists.txt does not exist.")
        message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
    endif()
    add_subdirectory(mt4g)
endif()
