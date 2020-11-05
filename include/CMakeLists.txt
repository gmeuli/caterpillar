add_library(caterpillar INTERFACE)
target_include_directories(caterpillar INTERFACE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(caterpillar INTERFACE mockturtle tweedledum json)

if(CATERPILLAR_Z3)
    target_compile_definitions(caterpillar INTERFACE USE_Z3) # -DUSE_Z3
    if(Z3_INCLUDE_DIR)
        target_include_directories(caterpillar INTERFACE ${Z3_INCLUDE_DIR})
    endif()
    if(Z3_LINK_DIR)
        find_library (
            Z3
            NAMES z3 libz3 # what to look for
            HINTS "${Z3_LINK_DIR}" # where to look
            NO_DEFAULT_PATH # do not search system default paths
        )
        #link_directories(caterpillar INTERFACE ${Z3_LINK_DIR})
    endif()

    target_link_libraries(caterpillar INTERFACE ${Z3})          # -lz3
endif()
