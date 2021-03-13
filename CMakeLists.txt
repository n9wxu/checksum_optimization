# What CMake to start at
cmake_minimum_required(VERSION 3.12)

# Include the subsidiary .cmake file to get the SDK
include(pico_sdk_import.cmake)
 
# Set the name and version of the project
project(PicoTest VERSION 1.0.0)
 
# Link the Project to a source file (step 4.6)
add_executable(PicoTest main.c rfc1071.c usGenerateChecksum.c usGenerateChecksum2.c usGenerateChecksum32.c)

target_link_libraries(PicoTest pico_stdlib)

# Initalise the SDK
pico_sdk_init()
 
# Enable USB, UART output
pico_enable_stdio_usb(PicoTest 1)
pico_enable_stdio_uart(PicoTest 0)
 
# Enable extra outputs (SWD?)
pico_add_extra_outputs(PicoTest)
