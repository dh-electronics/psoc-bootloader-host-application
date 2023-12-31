# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: (c) 2018-2023 DH electronics GmbH

cmake_minimum_required (VERSION 3.0)
project (psoc-bootloader-tool)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-std=c++11)
add_compile_options(-fPIC)

add_definitions(
    -DCALL_CON=
)

set(SOURCES
    main.cpp
    SpiDevice.cpp
    Bootloader.cpp
    cybootloaderutils/cybtldr_api2.c
    cybootloaderutils/cybtldr_api.c
    cybootloaderutils/cybtldr_command.c
    cybootloaderutils/cybtldr_parse.c
)

set(HEADERS
    CommDevice.h
    SpiDevice.h
    Bootloader.h
    cybootloaderutils/cybtldr_utils.h
    cybootloaderutils/cybtldr_api2.h
    cybootloaderutils/cybtldr_api.h
    cybootloaderutils/cybtldr_command.h
    cybootloaderutils/cybtldr_parse.h
)

file(GLOB_RECURSE OTHER . *.cfg *.txt *.sh)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/
        ${CMAKE_CURRENT_SOURCE_DIR}/cybootloaderutils
)

target_link_libraries(${PROJECT_NAME}
    dhcom-hal
)

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
    RUNTIME
    DESTINATION bin
    COMPONENT bin
)
