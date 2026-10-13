cmake_minimum_required(VERSION 3.22)

# Static MSVC runtime -> no Visual C++ redistributable needed on Windows.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS version")
# Some dependencies still declare very old CMake minimums.
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "")
# Make dependencies with old policy settings honour CMAKE_MSVC_RUNTIME_LIBRARY too.
set(CMAKE_POLICY_DEFAULT_CMP0091 NEW)

project(Soundtag VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

include(FetchContent)

# ---------------------------------------------------------------- JUCE
# Pass -DFETCHCONTENT_SOURCE_DIR_JUCE=/path/to/JUCE to use a local checkout.
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.4
    GIT_SHALLOW    TRUE)
FetchContent_MakeAvailable(JUCE)

# ---------------------------------------------------------------- TagLib (static)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING     OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES    OFF CACHE BOOL "" FORCE)
set(BUILD_BINDINGS    OFF CACHE BOOL "" FORCE)
set(WITH_ZLIB         OFF CACHE BOOL "" FORCE)
FetchContent_Declare(taglib
    GIT_REPOSITORY https://github.com/taglib/taglib.git
    GIT_TAG        v2.1.1
    GIT_SHALLOW    TRUE
    GIT_SUBMODULES_RECURSE TRUE)
FetchContent_MakeAvailable(taglib)
set(TAGLIB_INCLUDE_DIRS
    "${taglib_SOURCE_DIR}/taglib"
    "${taglib_SOURCE_DIR}/taglib/toolkit"
    "${taglib_BINARY_DIR}")

# ---------------------------------------------------------------- App
juce_add_gui_app(Soundtag
    PRODUCT_NAME          "Soundtag"
    COMPANY_NAME          "Soundtag"
    BUNDLE_ID             "io.github.shenphenchoedronhue.soundtag"
    VERSION               ${PROJECT_VERSION}
    ICON_BIG              "${CMAKE_CURRENT_SOURCE_DIR}/packaging/icon.png"
    MICROPHONE_PERMISSION_ENABLED FALSE)

target_sources(Soundtag PRIVATE
    Source/Main.cpp
    Source/MainComponent.cpp
    Source/TagIO.cpp)

target_include_directories(Soundtag PRIVATE ${TAGLIB_INCLUDE_DIRS})

target_compile_definitions(Soundtag PRIVATE
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:Soundtag,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:Soundtag,JUCE_VERSION>"
    JUCE_USE_CURL=0
    JUCE_WEB_BROWSER=0
    JUCE_LOAD_CURL_SYMBOLS_LAZILY=1
    JUCE_REPORT_APP_USAGE=0
    JUCE_STRICT_REFCOUNTEDPOINTER=1
    TAGLIB_STATIC)

target_link_libraries(Soundtag
    PRIVATE
        juce::juce_gui_basics
        tag
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

if(MSVC)
    target_compile_options(Soundtag PRIVATE /utf-8 $<$<CONFIG:Release>:/O1>)
else()
    target_compile_options(Soundtag PRIVATE $<$<CONFIG:Release>:-Os>)
    if(APPLE)
        target_link_options(Soundtag PRIVATE $<$<CONFIG:Release>:-Wl,-dead_strip>)
    else()
        target_compile_options(Soundtag PRIVATE -ffunction-sections -fdata-sections)
        target_link_options(Soundtag PRIVATE $<$<CONFIG:Release>:-Wl,--gc-sections>)
    endif()
endif()

# ---------------------------------------------------------------- Tests (optional)
option(SOUNDTAG_BUILD_TESTS "Build the TagIO round-trip test" OFF)
if(SOUNDTAG_BUILD_TESTS)
    juce_add_console_app(SoundtagTests PRODUCT_NAME "SoundtagTests")
    target_sources(SoundtagTests PRIVATE tests/TagIOTest.cpp Source/TagIO.cpp)
    target_include_directories(SoundtagTests PRIVATE ${TAGLIB_INCLUDE_DIRS})
    target_compile_definitions(SoundtagTests PRIVATE JUCE_USE_CURL=0 JUCE_WEB_BROWSER=0 TAGLIB_STATIC)
    target_link_libraries(SoundtagTests PRIVATE juce::juce_core tag PUBLIC juce::juce_recommended_config_flags)
    if(MSVC)
        target_compile_options(SoundtagTests PRIVATE /utf-8)
    endif()
endif()

# ---------------------------------------------------------------- Install / packaging
if(APPLE)
    install(TARGETS Soundtag BUNDLE DESTINATION . COMPONENT app)
elseif(WIN32)
    install(TARGETS Soundtag RUNTIME DESTINATION . COMPONENT app)
else()
    install(TARGETS Soundtag RUNTIME DESTINATION bin COMPONENT app)
    install(FILES packaging/linux/soundtag.desktop DESTINATION share/applications COMPONENT app)
    install(FILES packaging/icon.png DESTINATION share/icons/hicolor/512x512/apps RENAME soundtag.png COMPONENT app)
endif()

set(CPACK_PACKAGE_NAME                "Soundtag")
set(CPACK_PACKAGE_VENDOR              "Soundtag")
set(CPACK_PACKAGE_VERSION             ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Soundtag - en lille editor til metadata i lydfiler")
set(CPACK_PACKAGE_INSTALL_DIRECTORY   "Soundtag")
set(CPACK_PACKAGE_EXECUTABLES         "Soundtag" "Soundtag")
set(CPACK_PACKAGE_CONTACT             "shenphenchoedron-hue <shenphenchoedron-hue@users.noreply.github.com>")
# Package only our own files (not JUCE/TagLib headers and tools).
set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};Soundtag;app;/")
set(SOUNDTAG_PKG_SUFFIX "" CACHE STRING "Suffix used in package file names (fx ubuntu22.04)")

if(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_DMG_VOLUME_NAME "Soundtag")
    set(CPACK_PACKAGE_FILE_NAME "Soundtag-${PROJECT_VERSION}-macOS")
elseif(WIN32)
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
    set(CPACK_NSIS_DISPLAY_NAME "Soundtag")
    set(CPACK_NSIS_PACKAGE_NAME "Soundtag")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "Soundtag.exe")
    set(CPACK_NSIS_MUI_FINISHPAGE_RUN "Soundtag.exe")
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_CREATE_DESKTOP_LINKS "Soundtag")
    set(CPACK_PACKAGE_FILE_NAME "Soundtag-${PROJECT_VERSION}-Windows-Setup")
else()
    set(CPACK_GENERATOR "DEB")
    set(CPACK_STRIP_FILES ON)
    set(CPACK_DEBIAN_PACKAGE_NAME "soundtag")
    set(CPACK_DEBIAN_PACKAGE_SECTION "sound")
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    # JUCE loads X11 libraries at runtime, so list them explicitly.
    set(CPACK_DEBIAN_PACKAGE_DEPENDS
        "libx11-6, libxext6, libxinerama1, libxrandr2, libxcursor1, libxrender1, libfreetype6, libfontconfig1")
    set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "zenity")
    set(CPACK_DEBIAN_FILE_NAME "soundtag_${PROJECT_VERSION}_${SOUNDTAG_PKG_SUFFIX}_amd64.deb")
endif()

include(CPack)
