add_library(caterpillar INTERFACE)
target_include_directories(caterpillar INTERFACE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(caterpillar INTERFACE ez kitty lorina sparsepp percy mockturtle tweedledum)
