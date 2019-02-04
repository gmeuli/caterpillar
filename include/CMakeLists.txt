add_library(caterpillar INTERFACE)
target_include_directories(caterpillar INTERFACE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(caterpillar INTERFACE mockturtle tweedledum)

if(CATERPILLAR_Z3)
    target_link_libraries(caterpillar INTERFACE z3)          # -lz3
    target_compile_definitions(caterpillar INTERFACE USE_Z3) # -DUSE_Z3
endif()
