# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

# set(CMAKE_C_CPPLINT "cpplint;--exclude=$ENV{IDF_PATH}/**;--exclude=../main/linenoise_no_echo.c;--exclude=dummy_main_src.c")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(testbed C)

find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_custom_target(doc ALL COMMAND ${DOXYGEN_EXECUTABLE} "docs/doxyfile"
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        COMMENT "Building user's documentation into ${PROJECT_SOURCE_DIR}/docs/compiled"
    )
endif()


