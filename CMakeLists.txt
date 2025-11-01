# --------------------------------------------------------------------------
# Dependencies
# --------------------------------------------------------------------------

find_package(yat CONFIG REQUIRED)

# --------------------------------------------------------------------------
# Collect sources and includes
# --------------------------------------------------------------------------

file(GLOB_RECURSE sources src/*.cpp)
set(includedirs
    include/
    include/si-com/
    include/si-cam/
    include/si-ans/
)

# --------------------------------------------------------------------------
# Target
# --------------------------------------------------------------------------

add_library(limaspectralinstrument ${sources})

target_include_directories(limaspectralinstrument
    PUBLIC 
        ${includedirs}
)

target_link_directories(limaspectralinstrument
    PRIVATE
        ${CMAKE_SOURCE_DIR}/slsDetectorPackage/bin/libSlsDetector.a
        ${CMAKE_SOURCE_DIR}/slsDetectorPackage/bin/libSlsReceiver.a
)

target_link_libraries(limaspectralinstrument 
    PUBLIC
        limacore
        yat::yat
)

if (CONAN_LIBCXX STREQUAL "libstdc++")
    target_compile_features(limaspectralinstrument
        PRIVATE
            cxx_std_11
    )
endif()

limatools_set_library_soversion(limaspectralinstrument "VERSION")

# --------------------------------------------------------------------------
# Compile definitions
# --------------------------------------------------------------------------

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" SPECTRAL_VERSION_RAW)
string(STRIP "${SPECTRAL_VERSION_RAW}" SPECTRAL_VERSION)

target_compile_definitions(limaspectralinstrument
    PUBLIC
        SPECTRAL_NAME=SpectralInstrument
        SPECTRAL_VERSION=${SPECTRAL_VERSION}
        SPECTRAL_ENABLED
)

message(STATUS "Camera enabled: SpectralInstrument ${SPECTRAL_VERSION}")

# --------------------------------------------------------------------------
# Install
# --------------------------------------------------------------------------

install(
    TARGETS limaspectralinstrument 
    LIBRARY DESTINATION lib
)
