cmake_minimum_required(VERSION 3.20)
project(bttb_cpp CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Package pkg-config
find_package(PkgConfig REQUIRED)

# Check for GTK 4
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

# Source files
set(SOURCES
    src/main.cpp
    src/bttb_logic.cpp
    src/ui_main.cpp
    src/ui_settings.cpp
    src/ui_iso.cpp
)

# Executable
add_executable(bttb ${SOURCES})

# Target Include Directories
target_include_directories(bttb PRIVATE src)

# Link Libraries
target_link_libraries(bttb PRIVATE PkgConfig::GTK4)

# Test target
add_executable(test_solver src/test_solver.cpp src/bttb_logic.cpp)
target_include_directories(test_solver PRIVATE src)

# Install targets
install(TARGETS bttb DESTINATION bin)
install(FILES README.md LICENSE DESTINATION share/doc/bttb)

# CPack packaging configuration
set(CPACK_PACKAGE_NAME "bttb-cpp")
set(CPACK_PACKAGE_VENDOR "BurnToTheBrim")
set(CPACK_PACKAGE_VERSION "3.3.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Burn to the Brim (C++ Port)")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_GENERATOR "TGZ")

# Source package exclusions
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES
    "/build/"
    "/.git/"
    "\\\\.git/"
    "build/"
)

include(CPack)



