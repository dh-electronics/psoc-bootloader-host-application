cmake_minimum_required (VERSION 2.6)
project (bootloader-tool)

include_directories(
    .
    cybootloaderutils
)

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
target_link_libraries(${PROJECT_NAME}
    DHCOM_HAL
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)
