# Поддержка Qt Framework
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC_OPTIONS -no-compress)
find_package(Qt5Core)
set(ignoreMe "${QT_QMAKE_EXECUTABLE}")
add_compile_definitions(QT_DEPRECATED_WARNINGS)