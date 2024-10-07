# CMakeList.txt : CMake project for example, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.20.0)

project ("example")

find_package(areg CONFIG REQUIRED)
# Add source to this project's executable.
add_executable (example example.cpp)
set_property(TARGET example PROPERTY CXX_STANDARD 17)
target_link_libraries(example PRIVATE areg::areg)
