cmake_minimum_required(VERSION 3.10)
project(Ortholidar)

set(CMAKE_CXX_STANDARD 17)

add_executable(Ortholidar src/main.cpp src/containers/Point3d.cpp src/containers/Point3d.h src/KDTree.cpp src/KDTree.h src/Timer.cpp src/Timer.h src/containers/Vector3d.cpp src/containers/Vector3d.h src/io/Las.cpp src/io/Las.h)


target_link_libraries(Ortholidar laslib stdc++ m)
target_include_directories(Ortholidar PUBLIC /usr/include/laslib)

find_package(PkgConfig REQUIRED)
pkg_check_modules(DEPS REQUIRED eigen3)
target_link_libraries(Ortholidar ${DEPS_LIBRARIES})
target_include_directories(Ortholidar PUBLIC ${DEPS_INCLUDE_DIRS})
target_compile_options(Ortholidar PUBLIC ${DEPS_CFLAGS_OTHER})
