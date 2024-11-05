# CMakeList.txt : include source and link with 'areg::areg'
cmake_minimum_required(VERSION 3.20.0)

project ("example" CXX)

find_package(areg CONFIG REQUIRED)
# Add source to this project's executable.
add_executable (example example.cpp)
target_link_libraries(example PRIVATE areg::areg)
