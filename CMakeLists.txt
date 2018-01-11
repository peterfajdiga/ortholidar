cmake_minimum_required(VERSION 3.10)
project(ortholidar)

set(CMAKE_CXX_STANDARD 17)

add_executable(ortholidar src/main.cpp src/containers/Point3d.cpp src/containers/Point3d.h src/operations/KDTree.cpp src/operations/KDTree.h src/Timer.cpp src/Timer.h src/containers/Vector3d.cpp src/containers/Vector3d.h src/io/Las.cpp src/io/Las.h src/containers/Color.h src/operations/misc.cpp src/operations/misc.h src/operations/normals.cpp src/operations/normals.h)


target_link_libraries(ortholidar laslib stdc++ m)
target_include_directories(ortholidar PUBLIC /usr/include/laslib)

find_package(PkgConfig REQUIRED)
pkg_check_modules(DEPS REQUIRED eigen3 libpng)
target_link_libraries(ortholidar ${DEPS_LIBRARIES})
target_include_directories(ortholidar PUBLIC ${DEPS_INCLUDE_DIRS})
target_compile_options(ortholidar PUBLIC ${DEPS_CFLAGS_OTHER})
