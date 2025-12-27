cmake_minimum_required(VERSION 3.22)

project(Aura VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# AAX SDK Path
set(AAX_SDK_PATH "C:/aax-sdk-2-9-0" CACHE PATH "Path to AAX SDK")

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

# Set AAX SDK path
juce_set_aax_sdk_path(${AAX_SDK_PATH})

juce_add_plugin(Aura
    COMPANY_NAME "SeshNx"
    PLUGIN_MANUFACTURER_CODE Sesh
    PLUGIN_CODE Aura
    FORMATS VST3 AU Standalone AAX
    PRODUCT_NAME "SeshNx Aura"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
)

target_sources(Aura
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/DSP/RoomReverb.cpp
        Source/DSP/EarlyReflections.cpp
        Source/DSP/DampingFilter.cpp
        Source/Utils/Parameters.cpp
        Source/Utils/PresetManager.cpp
)

target_include_directories(Aura
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/DSP
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/UI
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/Utils
)

# Binary resources (logo)
juce_add_binary_data(Aura_BinaryData
    SOURCES
        Resources/company_logo.png
    HEADER_NAME BinaryData.h
)

target_compile_definitions(Aura
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(Aura
    PRIVATE
        Aura_BinaryData
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# ==============================================================================
# Tests (optional - enable with -DAURA_BUILD_TESTS=ON)
# ==============================================================================
option(AURA_BUILD_TESTS "Build unit tests" OFF)

if(AURA_BUILD_TESTS)
    enable_testing()

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
        GIT_SHALLOW TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(Aura_Tests
        Tests/RoomReverbTests.cpp
        Tests/DampingFilterTests.cpp
        Source/DSP/RoomReverb.cpp
        Source/DSP/EarlyReflections.cpp
        Source/DSP/DampingFilter.cpp
        Source/Utils/Parameters.cpp
    )

    target_include_directories(Aura_Tests
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Source
            ${CMAKE_CURRENT_SOURCE_DIR}/Source/DSP
            ${CMAKE_CURRENT_SOURCE_DIR}/Source/Utils
    )

    target_link_libraries(Aura_Tests
        PRIVATE
            GTest::gtest_main
            juce::juce_audio_processors
            juce::juce_audio_utils
            juce::juce_dsp
    )

    target_compile_definitions(Aura_Tests
        PRIVATE
            JUCE_WEB_BROWSER=0
            JUCE_USE_CURL=0
            JUCE_STANDALONE_APPLICATION=1
    )

    include(GoogleTest)
    gtest_discover_tests(Aura_Tests)
endif()
