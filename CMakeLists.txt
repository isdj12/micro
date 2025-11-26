cmake_minimum_required(VERSION 3.16)

set(_bootloader_dir "${CMAKE_CURRENT_LIST_DIR}/bootloader_components")
if(DEFINED EXTRA_COMPONENT_DIRS)
    list(APPEND EXTRA_COMPONENT_DIRS "${_bootloader_dir}")
else()
    set(EXTRA_COMPONENT_DIRS "${_bootloader_dir}")
endif()

# Tell bootloader to also use our custom components
set(BOOTLOADER_EXTRA_COMPONENT_DIRS "${_bootloader_dir}")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(my_bootloader_project)

