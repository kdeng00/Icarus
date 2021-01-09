cmake_minimum_required(VERSION 3.15)


set(SOFTWARE_DESCRIPTION
    "A music streaming Web API that provides a way for users to stream their personal music collection")
set(SOFTWARE_VERSION
    "0.1.9")


set(VCPKG_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/icarus/3rdparty/vcpkg)

message("The vcpkg root path ${VCPKG_ROOT_PATH}")

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(TOOLCHAIN_PATH ${VCPKG_ROOT_PATH}/scripts/buildsystems/vcpkg.cmake)

    set(CMAKE_TOOLCHAIN_FILE
        "${TOOLCHAIN_PATH}"
        CACHE STRING "Vcpkg toolchain file")

    message("Using default toolchain file")
endif()


project(icarus VERSION ${SOFTWARE_VERSION} DESCRIPTION ${SOFTWARE_DESCRIPTION} LANGUAGES CXX)


add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/icarus_lib)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/icarus)
