cmake_minimum_required(VERSION 3.10)
project(tst_unireftest_mnl VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PRODUCTNAME "tst_unireftest_mnl")
set(DESCRIPTION "tst_unireftest_mnl")
set(COMPANY     "RTPTechGroup")
set(COPYRIGHT   "Copyright (C) RTPTechGroup")
set(LIBRARIES
    sqlextension
)
include(../manual.cmake)
include("${CMAKE_SOURCE_DIR}/cmake/qt.cmake")

find_package(Qt5Core)
target_link_libraries(${PROJECT_NAME} Qt5::Core)

find_package(Qt5Widgets)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)

find_package(Qt5Sql)
target_link_libraries(${PROJECT_NAME} Qt5::Sql)
