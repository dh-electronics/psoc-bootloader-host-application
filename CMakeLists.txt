cmake_minimum_required (VERSION 2.6)
project (bootloader-tool)

set(DHCOM_HAL_INCLUDE ../DHCOM_HAL/include CACHE PATH "DHCOM_HAL include files")

include_directories(
    .
    cybootloaderutils
    ${DHCOM_HAL_INCLUDE}
)

set(SOURCES
    main.cpp
    cybtldr_api2.c
    cybtldr_api.c
    cybtldr_command.c
    cybtldr_parse.c
)

set(HEADERS
    cybtldr_utils.h
    cybtldr_api2.h
    cybtldr_api.h
    cybtldr_command.h
    cybtldr_parse.h
)

file(GLOB_RECURSE OTHER . *.cfg *.txt *.sh)

find_library(DHCOM_HAL DHCOM_HAL)

