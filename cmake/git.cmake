if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
    execute_process(
        COMMAND git rev-list -1 HEAD -- .
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(
        COMMAND git describe --tags --always ${GIT_COMMIT}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(
        COMMAND git show -s --format=format:\"%ci\" ${GIT_COMMIT_HASH}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_DATE_OUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    string(REPLACE "\"" "" GIT_DATE "${GIT_DATE_OUT}")

else(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  set(GIT_DATE "")
  set(GIT_COMMIT_HASH "")
endif(EXISTS "${CMAKE_SOURCE_DIR}/.git")

configure_file(
  ${CMAKE_SOURCE_DIR}/cmake/version.h.in
  version.h
)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
