cmake_minimum_required (VERSION 3.15 FATAL_ERROR)

project (FoleysVideoEditor VERSION 0.1.0 LANGUAGES CXX OBJC C)

juce_add_gui_app (${PROJECT_NAME}
	PRODUCT_NAME ${PROJECT_NAME}
	VERSION "${PROJECT_VERSION}"
	MICROPHONE_PERMISSION_ENABLED ON
	CAMERA_PERMISSION_ENABLED ON
	SEND_APPLE_EVENTS_PERMISSION_ENABLED OFF
	DOCUMENT_BROWSER_ENABLED ON
	ICON_BIG "${CMAKE_CURRENT_LIST_DIR}/../Resources/FF-Logo.png"
	COMPANY_COPYRIGHT "2019 by Foleys Finest Audio"
	COMPANY_NAME "Foleys Finest Audio"
	COMPANY_WEBSITE "https://foleysfinest.com"
	COMPANY_EMAIL "info@foleysfinest.com"
	PLUGINHOST_AU TRUE)


set (sourceDir "${CMAKE_CURRENT_LIST_DIR}/Source")

target_sources (${PROJECT_NAME} PRIVATE
	"${sourceDir}/Main.cpp"
	"${sourceDir}/ClipProperties.h"
	"${sourceDir}/ClipProperties.cpp"
	"${sourceDir}/Library.h"
	"${sourceDir}/Library.cpp"
	"${sourceDir}/MainComponent.h"
	"${sourceDir}/MainComponent.cpp"
	"${sourceDir}/Player.h"
	"${sourceDir}/Player.cpp"
	"${sourceDir}/PlayerWindow.h"
	"${sourceDir}/PlayerWindow.cpp"
	"${sourceDir}/ProcessorComponent.h"
	"${sourceDir}/ProcessorComponent.cpp"
	"${sourceDir}/Properties.h"
	"${sourceDir}/Properties.cpp"
	"${sourceDir}/RenderDialog.h"
	"${sourceDir}/RenderDialog.cpp"
	"${sourceDir}/TimeLine.h"
	"${sourceDir}/TimeLine.cpp"
	"${sourceDir}/TransportControl.h"
	"${sourceDir}/TransportControl.cpp")

target_link_libraries (${PROJECT_NAME} PRIVATE
	foleys_video_engine
	ff_meters
	juce_audio_utils 
	juce_opengl
	juce_gui_extra
	juce_cryptography
	juce_audio_processors
	juce_audio_devices
	juce_audio_formats)

target_compile_definitions (${PROJECT_NAME} PRIVATE
	JUCE_STRICT_REFCOUNTEDPOINTER=1
	JUCE_PLUGINHOST_VST3=1
	JUCE_PLUGINHOST_LADSPA=1
	FOLEYS_USE_OPENGL=0
	FF_AUDIO_ALLOW_ALLOCATIONS_IN_MEASURE_BLOCK=0
	FOLEYS_REPORT_USAGE=0)

