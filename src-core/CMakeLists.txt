cmake_minimum_required(VERSION 3.0.0)
include(CheckLibraryExists)
include(CheckIncludeFileCXX)
include(CheckCSourceCompiles)
include(CMakePushCheckState)
include(CheckCCompilerFlag)

file(GLOB_RECURSE SatDump_core_CPPS *.cpp
    libs/aec/*.c
    libs/predict/*.c
    libs/bzlib/*.c
    libs/miniz/*.c
    libs/libtom/*.c
    libs/jpeg/*.c
    libs/jpeg12/*.c
    libs/dlfcn/dlfcn.c
    common/map/maidenhead.c
    libs/correct/*.c
    libs/openjp2/*.c
    libs/deepspace-turbo/*.c
    libs/bzlib/*.c
    libs/tiny-regex-c/*.c
    libs/lua/onelua.c
)

find_package(PkgConfig)

add_library(satdump_core SHARED ${SatDump_core_CPPS})
target_include_directories(satdump_core PUBLIC .)

# Set the resources path
if(MSVC OR BUILD_MSVC)
    target_compile_definitions(satdump_core PUBLIC RESOURCES_PATH="./")
    target_compile_definitions(satdump_core PUBLIC LIBRARIES_PATH="./")
else()
    target_compile_definitions(satdump_core PUBLIC RESOURCES_PATH="${CMAKE_INSTALL_PREFIX}/share/satdump/")
    target_compile_definitions(satdump_core PUBLIC LIBRARIES_PATH="${CMAKE_INSTALL_PREFIX}/lib/satdump/")
endif()

# We have to... For MSVC
if(BUILD_OPENCL)
    target_compile_definitions(satdump_core PUBLIC USE_OPENCL=1)
endif()

# Threads
find_package(Threads REQUIRED)
target_link_libraries(satdump_core PUBLIC Threads::Threads)

# OpenCL if we found it
if(BUILD_OPENCL)
    target_link_libraries(satdump_core PUBLIC ${OpenCL_LIBRARIES})
endif()

# OpenMP Linking for macOS
if(APPLE AND BUILD_OPENMP)
    target_link_libraries(satdump_core PUBLIC omp)
endif()

if(BUILD_ZIQ2)
    target_compile_definitions(satdump_core PUBLIC BUILD_ZIQ2="1")
endif()

if(MSVC)
    target_link_libraries(satdump_core PUBLIC shlwapi)
    target_link_libraries(satdump_core PUBLIC volk.dll)
    target_link_libraries(satdump_core PUBLIC libpng16.dll)
    target_link_libraries(satdump_core PUBLIC fftw3f.dll)
    target_link_libraries(satdump_core PUBLIC nng.dll)

    if(BUILD_ZIQ)
        target_link_libraries(satdump_core PUBLIC zstd.dll)
        target_compile_definitions(satdump_core PUBLIC BUILD_ZIQ="1")
    endif()

    target_include_directories(satdump_core PUBLIC libs/lua)
    target_link_libraries(satdump_core PUBLIC ws2_32)
elseif(ANDROID)
    # VOLK
    target_link_libraries(satdump_core PUBLIC volk cpufeatures m log dl)
    target_include_directories(satdump_core PUBLIC ../volk_includes)

    # LibPNG
    target_link_libraries(satdump_core PUBLIC png zlib)

    # FFTW3
    target_link_libraries(satdump_core PUBLIC fftw)

    # NNG
    target_link_libraries(satdump_core PUBLIC nng mbedtls mbedtlscrypto mbedtlsx509)

    # Libzstd
    target_link_libraries(satdump_core PUBLIC zstd)
    target_compile_definitions(satdump_core PUBLIC BUILD_ZIQ="1")

    target_include_directories(satdump_core PUBLIC libs/lua)
else()
    # VOLK
    pkg_check_modules(VOLK REQUIRED volk)
    check_cxx_source_compiles("#include <volk/volk_alloc.hh>\n int main() { return 0; }" VOLK_HAS_ALLOC)

    if(NOT VOLK_HAS_ALLOC)
        message(STATUS "VOLK does not have volk_alloc.hh, including volk_includes")
        target_include_directories(satdump_core PUBLIC ../volk_includes) # If we're running VOLK 1.0, include our own volk_alloc
    endif()

    target_include_directories(satdump_core PUBLIC ${volk_INCLUDE_DIRS})
    target_link_libraries(satdump_core PUBLIC volk)
    set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} volk) # We need to test stuff in volk later
    set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} ${volk_INCLUDE_DIRS}) # We need to test stuff in volk later

    # LibPNG
    if(WIN32 AND NOT MINGW)
        find_package(PNG CONFIG REQUIRED)
    else()
        find_package(PNG REQUIRED)
    endif()

    target_link_libraries(satdump_core PUBLIC PNG::PNG)

    # FFTW3
    if(APPLE)
        set(FFTW3f_DIR /usr/local/lib/cmake/fftw3f)
        find_package(FFTW3f CONFIG REQUIRED)
        target_link_libraries(satdump_core PUBLIC FFTW3::fftw3f)
    else()
        pkg_check_modules(FFTW3 REQUIRED fftw3f)
        target_link_libraries(satdump_core PUBLIC ${FFTW3_LIBRARIES})

        target_link_libraries(satdump_core PUBLIC stdc++fs)
    endif()

    # NNG
    find_library(NNG_LIBRARY nng REQUIRED)
    target_link_libraries(satdump_core PUBLIC ${NNG_LIBRARY})

    if(BUILD_ZIQ)
        # zstd
        find_library(ZSTD_LIBRARY zstd REQUIRED)
        target_link_libraries(satdump_core PUBLIC ${ZSTD_LIBRARY})
        target_compile_definitions(satdump_core PUBLIC BUILD_ZIQ="1")
    endif()

    # Lua
    target_include_directories(satdump_core PUBLIC libs/lua)

    # Armadillo
    find_package(Armadillo)

    if(ARMADILLO_FOUND)
        target_compile_definitions(satdump_core PUBLIC HAVE_ARMADILLO="1")
        target_link_libraries(satdump_core PUBLIC ${ARMADILLO_LIBRARIES})
        target_include_directories(satdump_core PUBLIC ${ARMADILLO_INCLUDE_DIRS})
        message("-- Building with Armadillo!")
    endif()

    # Dynamic linking
    target_link_libraries(satdump_core PUBLIC dl)

    include(CheckCXXSourceCompiles)

    if(NOT APPLE)
        # Check system has volk_32fc_x2_add_32fc
        check_cxx_source_compiles("#include <volk/volk.h>\n int main() { volk_32fc_x2_add_32fc(nullptr,nullptr,nullptr,0); return 0; }" VOLK_HAS_volk_32fc_x2_add_32fc)

        if(NOT VOLK_HAS_volk_32fc_x2_add_32fc)
            target_compile_definitions(satdump_core PUBLIC VOLK_NO_volk_32fc_x2_add_32fc="1")
        endif()
    endif()
endif()

if(APPLE)
    target_link_libraries(satdump_core PUBLIC "-framework CoreGraphics")
endif()

install(TARGETS satdump_core DESTINATION lib)
install(DIRECTORY . DESTINATION include/satdump FILES_MATCHING PATTERN "*.h")
install(DIRECTORY . DESTINATION include/satdump FILES_MATCHING PATTERN "*.hpp")