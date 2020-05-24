cmake_minimum_required(VERSION 3.10)
project(tst_dsvstream VERSION 1.0.0)

set(PRODUCTNAME "tst_dsvstream")
set(DESCRIPTION "tst_dsvstream")
set(COMPANY     "RTPTechGroup")
set(COPYRIGHT   "Copyright (C) RTPTechGroup")
set(LIBRARIES
    sqlextension
)
include(../auto.cmake)

find_package(Qt5Core)
target_link_libraries(${PROJECT_NAME} Qt5::Core)

find_package(Qt5Widgets)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)

find_package(Qt5Test)
target_link_libraries(${PROJECT_NAME} Qt5::Test)