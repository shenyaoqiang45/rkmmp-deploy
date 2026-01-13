cmake_minimum_required(VERSION 3.10)

project(rkmpp_mjpeg C)

# ==============================================================================
# Project Configuration
# ==============================================================================

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ==============================================================================
# Library Build
# ==============================================================================

# Add include directory
include_directories(include)

# Source files for the library
set(LIB_SOURCES
    src/encoder.c
    src/decoder.c
    src/utils.c
)

# Add the library
add_library(rkmpp_mjpeg SHARED ${LIB_SOURCES})

# Set library properties
set_target_properties(rkmpp_mjpeg PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
    PUBLIC_HEADER "include/rkmpp_mjpeg.h"
)

# Link libraries (pthread and potentially rkmpp)
# On a real system, you would find and link the rkmpp library here
# find_package(rkmpp REQUIRED)
# target_link_libraries(rkmpp_mjpeg rkmpp)
target_link_libraries(rkmpp_mjpeg pthread)

# ==============================================================================
# Test Cases Build
# ==============================================================================

# Enable testing
enable_testing()

# Add test executables
add_executable(test_encoder test/test_encoder.c)
add_executable(test_decoder test/test_decoder.c)
add_executable(test_integration test/test_integration.c)

# Link test executables to the library
target_link_libraries(test_encoder rkmpp_mjpeg)
target_link_libraries(test_decoder rkmpp_mjpeg)
target_link_libraries(test_integration rkmpp_mjpeg)

# Add tests to CTest
add_test(NAME EncoderTest COMMAND test_encoder)
add_test(NAME DecoderTest COMMAND test_decoder)
add_test(NAME IntegrationTest COMMAND test_integration)

# ==============================================================================
# Installation
# ==============================================================================

# Install library
install(TARGETS rkmpp_mjpeg
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Install public header
install(FILES include/rkmpp_mjpeg.h DESTINATION include)

# Install pkg-config file
# (This would be more complex in a real project)

# ==============================================================================
# Documentation
# ==============================================================================

# (Doxygen or other documentation generation can be added here)

# ==============================================================================
# Cleanup
# ==============================================================================

# (Add cleanup rules if needed)
