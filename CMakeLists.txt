cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME ST7789v_LCD)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

file(GLOB_RECURSE MY_SOURCES
        "MyCode/Src/*.c"
)

# LVGL 核心源码（排除 demo 和 examples 以减小体积）
file(GLOB_RECURSE LVGL_SOURCES
        "Middlewares/LVGL/lvgl/src/*.c"
)

file(GLOB_RECURSE LVGL_PORT_SOURCES
        "Middlewares/LVGL/lvgl/porting/lv_port_disp.c"
        # 如果有触摸，可添加 lv_port_indev.c
)

# GUI Guider 生成的 UI 代码
file(GLOB_RECURSE UI_SOURCES
        "Gui_App/generated/*.c"
        "Gui_App/custom/*.c"
)

# 收集所有头文件（仅用于 IDE 索引，不参与编译）
file(GLOB_RECURSE PROJECT_HEADERS
        "MyCode/Inc/*.h"
        "Middlewares/LVGL/lvgl"
        "Middlewares/LVGL/lvgl/src/*.h"
        "Middlewares/LVGL/lvgl/porting/*.h"
        "Gui_App/generated/*.h"
        "Gui_App/custom/*.h"
)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME}
        ${MY_SOURCES}
        ${LVGL_SOURCES}
        ${LVGL_PORT_SOURCES}
        ${UI_SOURCES}
        ${PROJECT_HEADERS})


# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
        MyCode/Inc

        # LVGL 核心 + 配置文件路径（确保 lv_conf.h 能被找到）
        Middlewares/LVGL
        Middlewares/LVGL/lvgl
        Middlewares/LVGL/lvgl/src
        Middlewares/LVGL/lvgl/porting

        # GUI Guider UI 头文件
        Gui_App/generated
        Gui_App/custom
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
