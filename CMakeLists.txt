cmake_minimum_required (VERSION 3.0.0)
project(wallaby)

if(NOT WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()


# TODO option to compile with Spirit sources and filesystem only
find_package(Boost REQUIRED)
if(NOT Boost_FOUND)
  message(FATAL_ERROR "Couldn't find Boost!")
endif()

include_directories(${Boost_INCLUDE_DIR})

include_directories(include)

add_library(${PROJECT_NAME} src/wallaby.cc)

target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

add_executable(basic_usage test/basic_usage.cc)
target_link_libraries(basic_usage ${PROJECT_NAME} ${Boost_LIBRARIES})

enable_testing()
add_test(basic_usage basic_usage)
