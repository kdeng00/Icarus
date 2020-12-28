cmake_minimum_required(VERSION 3.15)


set(SOFTWARE_DESCRIPTION
    "A music streaming Web API that provides a way for users to stream their personal music collection")
set(SOFTWARE_VERSION
    "0.1.9")


project(icarus VERSION ${SOFTWARE_VERSION} DESCRIPTION ${SOFTWARE_DESCRIPTION} LANGUAGES CXX)


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/icarus_lib)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/icarus)
