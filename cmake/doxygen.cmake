if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")

        set(DOXYGEN_IN  ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
        set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

        add_custom_target( doc_doxygen ALL
               COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
               WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
               COMMENT "Generating API documentation with Doxygen"
               VERBATIM)

        install(FILES "${DOXYGEN_OUTPUT_DIR}/${PROJECT_NAME}.qch"
            DESTINATION ${DOCPATH})
        install(DIRECTORY "${DOXYGEN_OUTPUT_DIR}/${PROJECT_NAME}"
            DESTINATION ${DOCPATH})
    else()
        message(WARNING "Doxygen not found, unable to generate documentation")
    endif()
endif()
