project(cpplazy-demo CXX)

add_executable (cpplazy-demo demo.cpp)
set_property(TARGET cpplazy-demo PROPERTY CXX_STANDARD 17)
target_include_directories(cpplazy-demo PRIVATE ../include)