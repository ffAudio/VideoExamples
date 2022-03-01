cmake_minimum_required (VERSION 3.15 FATAL_ERROR)

project (FoleysVideoEngineExamples VERSION 0.1.0 LANGUAGES CXX OBJC C)

set (CMAKE_SUPPRESS_REGENERATION TRUE)

if (NOT DEFINED ENV{CPM_SOURCE_CACHE})
    set (ENV{CPM_SOURCE_CACHE} "${CMAKE_CURRENT_LIST_DIR}/Cache")
endif()

include ("${CMAKE_CURRENT_LIST_DIR}/cmake/CPM.cmake")

CPMAddPackage (NAME foleys_video_engine
               GITHUB_REPOSITORY ffAudio/foleys_video_engine
               GIT_TAG origin/feature/cmake
               OPTIONS "FOLEYS_BUILD_TESTS OFF")

#juce_add_module ("${CMAKE_CURRENT_LIST_DIR}/modules/ff_meters")

add_subdirectory (VideoEditor)
add_subdirectory (VideoPlayer)
