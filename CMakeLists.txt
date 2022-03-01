cmake_minimum_required (VERSION 3.15 FATAL_ERROR)

project (FoleysVideoEngineExamples VERSION 0.1.0 LANGUAGES CXX OBJC C)

set (CMAKE_SUPPRESS_REGENERATION TRUE)

if (NOT DEFINED ENV{CPM_SOURCE_CACHE})
    set (ENV{CPM_SOURCE_CACHE} "${CMAKE_CURRENT_LIST_DIR}/Cache")
endif()

set (ENV{CPM_USE_NAMED_CACHE_DIRECTORIES} ON)

include ("${CMAKE_CURRENT_LIST_DIR}/cmake/CPM.cmake")

CPMAddPackage (NAME foleys_video_engine
               GITHUB_REPOSITORY ffAudio/foleys_video_engine
               GIT_TAG origin/feature/cmake
               OPTIONS "FOLEYS_BUILD_TESTS OFF")

CPMAddPackage (NAME ff_meters
               GITHUB_REPOSITORY ffAudio/ff_meters
               GIT_TAG origin/master
               DOWNLOAD_ONLY ON)

juce_add_module ("${ff_meters_SOURCE_DIR}")

add_subdirectory (VideoEditor)
add_subdirectory (VideoPlayer)
