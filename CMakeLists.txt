cmake_minimum_required(VERSION 3.20)
project(bttb_cpp C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find Package pkg-config
find_package(PkgConfig REQUIRED)

# Check for GTK 4
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)

# Target Include Directories
include_directories(src src/libpar3 src/blake3 src/leopard src/platform)

# Blake3 static library
add_library(blake3 STATIC
    src/blake3/blake3.c
    src/blake3/blake3_dispatch.c
    src/blake3/blake3_portable.c
    src/blake3/blake3_sse2.c
    src/blake3/blake3_sse41.c
    src/blake3/blake3_avx2.c
    src/blake3/blake3_avx512.c
)
set_source_files_properties(src/blake3/blake3_sse2.c   PROPERTIES COMPILE_OPTIONS "-msse2")
set_source_files_properties(src/blake3/blake3_sse41.c  PROPERTIES COMPILE_OPTIONS "-msse4.1")
set_source_files_properties(src/blake3/blake3_avx2.c   PROPERTIES COMPILE_OPTIONS "-mavx2")
set_source_files_properties(src/blake3/blake3_avx512.c PROPERTIES COMPILE_OPTIONS "-mavx512vl")

# Leopard static library
add_library(leopard STATIC
    src/leopard/leopard.cpp
    src/leopard/LeopardCommon.cpp
    src/leopard/LeopardFF8.cpp
    src/leopard/LeopardFF16.cpp
)
set_source_files_properties(src/leopard/LeopardFF8.cpp src/leopard/LeopardFF16.cpp PROPERTIES COMPILE_OPTIONS "-mssse3")

# Platform static library
if(UNIX)
    add_library(platform STATIC
        src/platform/linux/filelength.c
        src/platform/linux/filesearch.c
        src/platform/linux/get_absolute_path.c
    )
else()
    add_library(platform STATIC
        src/platform/windows/get_absolute_path.c
    )
endif()

# Libpar3 library
add_library(libpar3 STATIC
    src/libpar3/block_check.c
    src/libpar3/block_create.c
    src/libpar3/block_map.c
    src/libpar3/block_recover.c
    src/libpar3/common.c
    src/libpar3/file.c
    src/libpar3/galois16.c
    src/libpar3/galois8.c
    src/libpar3/hash.c
    src/libpar3/inside_zip.c
    src/libpar3/libpar3.c
    src/libpar3/libpar3_create.c
    src/libpar3/libpar3_extra.c
    src/libpar3/libpar3_inside.c
    src/libpar3/libpar3_verify.c
    src/libpar3/map.c
    src/libpar3/map_inside.c
    src/libpar3/map_simple.c
    src/libpar3/map_slide.c
    src/libpar3/packet_add.c
    src/libpar3/packet_make.c
    src/libpar3/packet_parse.c
    src/libpar3/read.c
    src/libpar3/reedsolomon16.c
    src/libpar3/reedsolomon8.c
    src/libpar3/reedsolomon.c
    src/libpar3/repair.c
    src/libpar3/verify.c
    src/libpar3/verify_check.c
    src/libpar3/write.c
    src/libpar3/write_inside.c
    src/libpar3/write_trial.c
)

# Source files for BTTB
set(SOURCES
    src/main.cpp
    src/bttb_logic.cpp
    src/ui_main.cpp
    src/ui_settings.cpp
    src/ui_iso.cpp
    src/ui_verify.cpp
)

# Executable
add_executable(bttb ${SOURCES})
target_include_directories(bttb PRIVATE src src/libpar3 src/blake3 src/leopard src/platform)
target_link_libraries(bttb PRIVATE PkgConfig::GTK4 libpar3 blake3 leopard platform)
target_link_options(bttb PRIVATE -static-libgcc -static-libstdc++)

# Test target
add_executable(test_solver src/test_solver.cpp src/bttb_logic.cpp)
target_include_directories(test_solver PRIVATE src src/libpar3 src/blake3 src/leopard src/platform)
target_link_libraries(test_solver PRIVATE libpar3 blake3 leopard platform)

# Install targets
install(TARGETS bttb DESTINATION bin)
install(FILES README.md LICENSE DESTINATION share/doc/bttb)
install(PROGRAMS src/bttb_embed.py DESTINATION bin)

# CPack packaging configuration
set(CPACK_PACKAGE_NAME "bttb-cpp")
set(CPACK_PACKAGE_VENDOR "BurnToTheBrim")
set(CPACK_PACKAGE_VERSION "4.2.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Burn to the Brim (C++ Port)")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_GENERATOR "TGZ;DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "BurnToTheBrim Team <support@sourceforge.net>")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Burn to the Brim C++ bin-packing optimizer")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS OFF)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.35), libglib2.0-0t64 | libglib2.0-0 (>= 2.28.0), libgtk-4-1 (>= 4.6.0)")

# Source package exclusions
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_SOURCE_IGNORE_FILES
    "/build/"
    "/.git/"
    "\\.git/"
    "build/"
)

include(CPack)
