cmake_minimum_required(VERSION 3.1)
project(sgre)

set(PYTHON_VERSION 2.7)
set(BOOST_PYTHONLIB python-py27)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ftemplate-depth=1024")
file(GLOB sgre_SRC_FILES "wrapper/*.cpp" "src/*.cpp" "libs/tess2/source/*.c")

find_package(PythonInterp ${PYTHON_VERSION} REQUIRED)
find_package(PythonLibs ${PYTHON_VERSION} REQUIRED)
find_package(Boost REQUIRED COMPONENTS ${BOOST_PYTHONLIB})

include_directories("${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/libs" "${PROJECT_SOURCE_DIR}/libs/tess2/include" "${PYTHON_INCLUDE_DIRS}" "${Boost_INCLUDE_DIRS}")
link_directories("${Boost_LIBRARY_DIR}")

add_library(sgre SHARED "${sgre_SRC_FILES}")
target_link_libraries(sgre "${Boost_LIBRARIES}" "${PYTHON_LIBRARIES}")

set_target_properties(sgre PROPERTIES PREFIX "")
