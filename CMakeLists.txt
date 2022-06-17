cmake_minimum_required (VERSION 3.15 FATAL_ERROR)

project (FoleysVideoEngineExamples 
            VERSION 0.1.0 
            LANGUAGES CXX OBJC C
            DESCRIPTION "Examples for the foleys_video_engine module"
            HOMEPAGE_URL "https://foleysfinest.com/foleys_video_engine/")

add_subdirectory (modules)

add_subdirectory (VideoEditor)
add_subdirectory (VideoPlayer)
