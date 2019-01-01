include("${CMAKE_SOURCE_DIR}/cmake/main.cmake")

# Места нахождения бинарных файлов
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BINARY_DIR}/${TESTPATH}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BINARY_DIR}/${TESTPATH}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BINARY_DIR}/${TESTPATH}")

# Сборка проекта
include("${CMAKE_SOURCE_DIR}/cmake/rpath.cmake")
add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} ${FORMS} ${RESOURCES})
add_test(${PROJECT_NAME} "${BINARY_DIR}/${TESTPATH}/${PROJECT_NAME}")
target_link_libraries(${PROJECT_NAME} ${LIBRARIES})
