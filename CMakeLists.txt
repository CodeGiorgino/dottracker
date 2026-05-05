cmake_minimum_required(VERSION 3.11)

project(dottracker
    VERSION 1.2.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configure files

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/version.hxx.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/version.hxx
    @ONLY)

# Target executable

file(GLOB_RECURSE SOURCE_FILES
    CONFIGURE_DEPENDS src/*.cxx)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_compile_options(${PROJECT_NAME}
    PRIVATE -Wall -Wextra -pedantic)

target_include_directories(${PROJECT_NAME}
    PRIVATE include)

target_include_directories(${PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/generated)

# Create symlink for tracked files in build directory

if(WIN32)
    get_filename_component(REAL_PATH
        ${CMAKE_CURRENT_SOURCE_DIR}/files REALPATH)
    string(REPLACE "/" "\\"
        TARGET_PATH
        ${REAL_PATH})

    execute_process(
        COMMAND cmd /C mklink /J files ${TARGET_PATH}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
else()
    execute_process(
        COMMAND ln -s ${CMAKE_CURRENT_SOURCE_DIR}/files ${CMAKE_CURRENT_BINARY_DIR}/files)
endif()
