cmake_minimum_required(VERSION 3.16)
project(TableKM VERSION 1.0 LANGUAGES CXX)

set(LANGUAGE CXX)
set(CXX_STANDARD_REQUIRED 17)

if(CMAKE_COMPILER_IS_GNUCXX)
    message(STATUS "GCC detected, adding compile flags")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif(CMAKE_COMPILER_IS_GNUCXX)

if(NOT EXISTS ${CMAKE_SOURCE_DIR}/KMTableLib)
    message(FATAL_ERROR "KMTableLib is required to build this project See https://github.com/keshav-sahu7/KMTableLib.git to get KMTableLib")
endif()


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(Qt6 COMPONENTS PrintSupport REQUIRED)

# src is included so that automatically generated ui_*.h files can include these headers.
include_directories(${CMAKE_SOURCE_DIR}/KMTableLib/include   ${Qt6Widgets_INCLUDE_DIRS}   src)
link_directories(${CMAKE_SOURCE_DIR}/KMTableLib/lib)

set (EXTERNAL_THEME_RESOURCE
    ${CMAKE_SOURCE_DIR}/themes/dark/external_theme.qrc
)

set(
    K_BUILD_DIR
    ${CMAKE_BUILD_TYPE}
)

add_subdirectory(src)
