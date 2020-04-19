# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

# set(CMAKE_C_CPPLINT "cpplint;--exclude=$ENV{IDF_PATH}/**;--exclude=../main/linenoise_no_echo.c;--exclude=dummy_main_src.c")

find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_custom_target(doxyDoc COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_BINARY_DIR}/docs/doxyfile"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Building user's documentation into doxyDoc build dir..."
    )
endif()

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(mvm C)
