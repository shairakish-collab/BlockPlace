cmake_minimum_required(VERSION 3.20)
project(random_block_placement CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    preloader_android
    GIT_REPOSITORY https://github.com/LiteLDev/preloader-android.git
    GIT_TAG 0.2.2   # pin to a real release tag - check the repo for the latest
)
FetchContent_MakeAvailable(preloader_android)

add_library(random_block_placement SHARED
    RandomBlockPlacementMod.cpp
)

target_link_libraries(random_block_placement PRIVATE preloader)

set_target_properties(random_block_placement PROPERTIES
    OUTPUT_NAME "random_block_placement"
    PREFIX "lib"
)

# Build for arm64-v8a only, per SDK requirements:
#   cmake -B build -G Ninja \
#     -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
#     -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-24
#   cmake --build build
