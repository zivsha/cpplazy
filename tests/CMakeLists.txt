project(cpplazy-tests CXX)
add_executable (cpplazy-tests main.cpp tests.cpp catch.hpp)
set_property(TARGET cpplazy-tests PROPERTY CXX_STANDARD 17)
target_include_directories(cpplazy-tests PRIVATE ../include)