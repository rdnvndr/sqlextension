cmake_minimum_required(VERSION 3.10)
project(tst_uuidtreemodel_mnl VERSION 1.0.0)

set(PRODUCTNAME "tst_uuidtreemodel_mnl")
set(DESCRIPTION "tst_uuidtreemodel_mnl")
set(COMPANY     "RTPTechGroup")
set(COPYRIGHT   "Copyright (C) RTPTechGroup")
set(LIBRARIES
    sqlextension
)
include(../manual.cmake)

find_package(Qt5Core)
target_link_libraries(${PROJECT_NAME} Qt5::Core)

find_package(Qt5Widgets)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets)

find_package(Qt5Sql)
target_link_libraries(${PROJECT_NAME} Qt5::Sql)

# Установка файла базы данных
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/tree.db"
     ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)
