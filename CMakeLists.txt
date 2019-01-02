cmake_minimum_required(VERSION 3.10)
project(guiextension VERSION 1.0.0)

add_subdirectory(src)
add_subdirectory(doc)

enable_testing()
add_subdirectory(tests EXCLUDE_FROM_ALL)
