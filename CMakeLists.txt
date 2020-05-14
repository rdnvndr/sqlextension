cmake_minimum_required(VERSION 3.10)
project(sqlextension VERSION 1.0.0)

add_subdirectory(src)
add_subdirectory(doc)

if ( ${CMAKE_TESTING_ENABLED} )
    add_subdirectory(tests)
endif()
