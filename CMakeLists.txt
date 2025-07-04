cmake_minimum_required(VERSION 3.16.0)

project(lemon LANGUAGES CXX)

## Add paths to check for cmake modules:
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# ==================================================================================
# CMake Macros
# ==================================================================================
include(FeatureSummary)

macro(LEMONLOG MSG)
    set(LEMON_BUILD_STATS "${LEMON_BUILD_STATS}${MSG}: ${${MSG}}\r\n")
endmacro()


option(EMBED_TRANSLATIONS "Embed translations" ON)
LEMONLOG(EMBED_TRANSLATIONS)
option(EMBED_DOCS "Embed documents" ON)
LEMONLOG(EMBED_DOCS)
option(ENABLE_XLS_EXPORT "XLS Result Export Support (Windows Only)" OFF)
LEMONLOG(ENABLE_XLS_EXPORT)
option(BUILD_DEB "Build deb format package" OFF)
LEMONLOG(BUILD_DEB)
option(BUILD_RPM "Build rpm format package" OFF)
LEMONLOG(BUILD_RPM)
option(ENABLE_LTO "Link-Time Optimization" ON)
LEMONLOG(ENABLE_LTO)
option(ENABLE_CCACHE "Ccache speed up compile (Only support Makefile and Ninja now)" OFF)
LEMONLOG(ENABLE_CCACHE)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/src/forms)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(LEMON_QT6 "Use Qt6 for Debugging" OFF)
LEMONLOG(LEMON_QT6)
if(LEMON_QT6)
    cmake_policy(SET CMP0072 NEW)
    set(LEMON_QT_MAJOR_VERSION 6)
    set(LEMON_QT_MINOR_VERSION 0)
    set(LEMON_QT_LIBNAME Qt6)
    set(QT_MAJOR_VERSION 6)
    set(QT_DEFAULT_MAJOR_VERSION 6)
    set(Qt_DIR ${Qt6_DIR})
    add_definitions(-DLEMON_QT6)
else()
    set(LEMON_QT_MAJOR_VERSION 5)
    set(LEMON_QT_MINOR_VERSION 15)
    set(LEMON_QT_LIBNAME Qt5)
    set(QT_MAJOR_VERSION 5)
    set(Qt_DIR ${Qt5_DIR})
endif()

add_definitions(-DQT_NO_FOREACH)

find_package(${LEMON_QT_LIBNAME} ${LEMON_QT_MAJOR_VERSION}.${LEMON_QT_MINOR_VERSION} COMPONENTS Core Gui Widgets REQUIRED)
list(APPEND LEMON_QT_LIBS ${LEMON_QT_LIBNAME}::Core ${LEMON_QT_LIBNAME}::Gui ${LEMON_QT_LIBNAME}::Widgets)

set(SINGLEAPPLICATION_DIR ${CMAKE_SOURCE_DIR}/3rdparty/SingleApplication)
set(QAPPLICATION_CLASS QApplication CACHE STRING "Inheritance class for SingleApplication")
add_subdirectory(3rdparty/SingleApplication)

# spdlog 
add_compile_definitions(SPDLOG_COMPILED_LIB)
add_compile_definitions(SPDLOG_BUILD_SHARED=OFF)
set(spdlog_DIR ${CMAKE_SOURCE_DIR}/3rdparty/spdlog)
add_subdirectory(3rdparty/spdlog)

if(WIN32)
    add_compile_options("/std:c++17")
    add_compile_definitions(UNICODE _UNICODE)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    set(GUI_TYPE WIN32)
endif()

if(UNIX AND NOT APPLE)
    find_program(LSB_RELEASE_EXEC lsb_release)
    execute_process(COMMAND ${LSB_RELEASE_EXEC} -is
        OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()
LEMONLOG(LSB_RELEASE_ID_SHORT)

if(ENABLE_CCACHE)
    find_program(CCACHE_PROGRAM ccache)
    if(CCACHE_PROGRAM AND UNIX)
        set(C_LAUNCHER   "${CCACHE_PROGRAM}")
        set(CXX_LAUNCHER "${CCACHE_PROGRAM}")

        if(CMAKE_GENERATOR STREQUAL "Xcode")
            message(STATUS "Ccache not supported by XCode, disabled")
        endif()
    endif()
endif()

# ==================================================================================
# Lemon Build Info
# ==================================================================================

file(STRINGS "${CMAKE_SOURCE_DIR}/makespec/VERSION" VERSION)
file(STRINGS "${CMAKE_SOURCE_DIR}/makespec/BUILDVERSION" BUILD_VERSION)
file(STRINGS "${CMAKE_SOURCE_DIR}/makespec/VERSIONSUFFIX" VERSION_SUFFIX)

set(VERSION_STRING "${VERSION}${VERSION_SUFFIX}")
set(VERSION_LIST ${VERSION})
string(REPLACE "." ";" VERSION_LIST ${VERSION_LIST})
separate_arguments(VERSION_LIST)

list(GET VERSION_LIST 0 CMAKE_PROJECT_VERSION_MAJOR)
list(GET VERSION_LIST 1 CMAKE_PROJECT_VERSION_MINOR)
list(GET VERSION_LIST 2 CPACK_PACKAGE_VERSION_PATCH)

add_definitions(-DLEMON_VERSION_MAJOR=${CMAKE_PROJECT_VERSION_MAJOR})
add_definitions(-DLEMON_VERSION_MINOR=${CMAKE_PROJECT_VERSION_MINOR})
add_definitions(-DLEMON_VERSION_BUGFIX=${CPACK_PACKAGE_VERSION_PATCH})
add_definitions(-DLEMON_VERSION_BUILD=${BUILD_VERSION})

add_definitions(-DLEMON_VERSION_STRING="${VERSION_STRING}")

message(" ")
message("LemonLime Version: ${VERSION_STRING}")
message("LemonLime Build Version: ${BUILD_VERSION}")
message("LemonLime Build Type: ${CMAKE_BUILD_TYPE}")
message("|----------------------------------------------------------------|")
message("| LemonLime,                                                     |")
message("| A tiny judging environment for OI contest.                     |")
message("| Based on C++ & Qt.                                             |")
message("| Licenced under GPLv3                                           |")
message("|                                                                |")
message("|----------------------------------------------------------------|")
message("| GitHub: https://github.com/Project-LemonLime/Project_LemonLime |")
message("| Welcome to contribute!                                         |")
message("|----------------------------------------------------------------|")
message(" ")

if(LEMON_BUILD_INFO)
    set(_LEMON_BUILD_INFO_STR_ "${LEMON_BUILD_INFO}")
elseif(DEFINED ENV{_LEMON_BUILD_INFO_})
    set(_LEMON_BUILD_INFO_STR_ "$ENV{_LEMON_BUILD_INFO_}")
else()
    set(_LEMON_BUILD_INFO_STR_ "LemonLime from manual build")
endif()

if(LEMON_BUILD_EXTRA_INFO)
    set(_LEMON_BUILD_EXTRA_INFO_STR_ "${LEMON_BUILD_EXTRA_INFO}")
elseif(DEFINED ENV{_LEMON_BUILD_EXTRA_INFO_})
    set(_LEMON_BUILD_EXTRA_INFO_STR_ "$ENV{_LEMON_BUILD_EXTRA_INFO_}")
else()
    set(_LEMON_BUILD_EXTRA_INFO_STR_ "LemonLime ${VERSION_STRING}:${BUILD_VERSION}")
endif()

set(LEMON_BUILD_INFO ${_LEMON_BUILD_INFO_STR_})
set(LEMON_BUILD_EXTRA_INFO ${_LEMON_BUILD_EXTRA_INFO_STR_})

add_definitions(-DLEMON_BUILD_INFO_STR="${_LEMON_BUILD_INFO_STR_}")
add_definitions(-DLEMON_BUILD_EXTRA_INFO_STR="${_LEMON_BUILD_EXTRA_INFO_STR_}")
message("LemonLime build info: ${_LEMON_BUILD_INFO_STR_}")
message("LemonLime build info extra: ${_LEMON_BUILD_EXTRA_INFO_STR_}")

if(LEMON_CONFIG_DIR)
    add_definitions(-DLEMON_CONFIG_DIR="${LEMON_CONFIG_DIR}")
    LEMONLOG(LEMON_CONFIG_DIR)
endif()

# ==================================================================================
# Lemon Base && Core
# ==================================================================================

include(cmake/lemon-base.cmake)
include(cmake/lemon-core.cmake)

# ==================================================================================
# Lemon Documents
# ==================================================================================

include(cmake/LemonDocs.cmake)
    
# ==================================================================================
# Lemon UI
# ==================================================================================

include(cmake/lemon-ui.cmake)
    
# ==================================================================================
# Lemon Translations
# ==================================================================================

include(cmake/LemonTranslations.cmake)

if(EMBED_TRANSLATIONS)
    add_definitions(-DLEMON_EMBED_TRANSLATIONS)
    configure_file(translations/translations.qrc ${CMAKE_BINARY_DIR} COPYONLY)
    set(LEMON_EMBED_TRANSLATION_QRC ${CMAKE_BINARY_DIR}/translations.qrc)
endif()

# Qt language translations
add_custom_target(lupdate
    COMMAND ${Qt_DIR}/../../../bin/lupdate
    ${LEMON_BASE_SOURCES}
    ${LEMON_CORE_SOURCES}
    ${LEMON_UI_SOURCES}
    ${LEMON_UI_FORMS}
    ${SINGLEAPPLICATION_SOURCES} -ts ${TRANSLATIONS_TS} -locations none -noobsolete
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
set_target_properties(lupdate PROPERTIES EXCLUDE_FROM_ALL TRUE)


# ==================================================================================
# Lemon Build
# ==================================================================================

set(QRC_RESOURCES
    resource.qrc
    ${LEMON_EMBED_DOC_QRC}
    )

set(LEMON_FULL_SOURCES
    ${LEMON_BASE_SOURCES}
    ${LEMON_CORE_SOURCES}
    ${LEMON_UI_SOURCES}
    assets/lemon.rc
    ${LEMON_UI_FORMS}
    ${LEMON_UI_RESOURCES}
    ${SINGLEAPPLICATION_SOURCES}
    ${LEMON_EMBED_TRANSLATION_QRC}
    ${QRC_RESOURCES}
    ${QM_FILES}
    )


# Set Apple MacOS Specific settings
if(APPLE)
    set(GUI_TYPE MACOSX_BUNDLE)
endif()

if(WIN32)
    add_executable(lemon ${GUI_TYPE} ${LEMON_FULL_SOURCES})
else()
    if(APPLE)
        add_executable(watcher_unix unix/watcher_macos_rlimit.cpp unix/watcher_macos_static.mm)
    else()
        add_executable(watcher_unix unix/watcher_linux_cgroup_v2.cpp unix/watcher_linux_static.cpp)
    endif()
    configure_file(unix/watcher.qrc ${CMAKE_BINARY_DIR} COPYONLY)
    list(APPEND LEMON_FULL_SOURCES ${CMAKE_BINARY_DIR}/watcher.qrc)
    add_executable(lemon ${GUI_TYPE} ${LEMON_FULL_SOURCES})
    add_dependencies(lemon watcher_unix)
endif()

target_include_directories(lemon PUBLIC ${CMAKE_SOURCE_DIR}/src)

if(WIN32 AND ENABLE_XLS_EXPORT)
    find_package(${LEMON_QT_LIBNAME} COMPONENTS AxContainer REQUIRED)
    list(APPEND LEMON_QT_LIBS ${LEMON_QT_LIBNAME}::AxContainer)
    add_definitions(-DENABLE_XLS_EXPORT)
endif()

target_precompile_headers(lemon PUBLIC ${CMAKE_SOURCE_DIR}/src/pch.h)

target_link_libraries(lemon PRIVATE ${LEMON_QT_LIBS} lemon-core lemon-base SingleApplication::SingleApplication spdlog::spdlog)

if(WIN32 AND NOT LEMON_QT6)
    target_link_libraries(lemon PRIVATE userenv)
endif()

if(ENABLE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT LTO_SUPPORTED OUTPUT error)

    if(LTO_SUPPORTED)
        set_property(TARGET lemon PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(STATUS "IPO / LTO not supported, disabled")
    endif()
endif()

# ==================================================================================
# Platform-dependent installation process and deployment
# ==================================================================================

if(APPLE)
    include(cmake/platforms/macos.cmake)
elseif(UNIX AND NOT APPLE AND NOT WIN32)
    include(cmake/platforms/linux.cmake)
elseif(WIN32)
    include(cmake/platforms/windows.cmake)
endif()

# ==================================================================================
# Lemon Package
# ==================================================================================

include(cmake/deployment.cmake)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

LEMONLOG(VERSION_STRING)
LEMONLOG(BUILD_VERSION)
LEMONLOG(CMAKE_BUILD_TYPE)
LEMONLOG(LEMON_BUILD_INFO)
LEMONLOG(LEMON_BUILD_EXTRA_INFO)
LEMONLOG(CMAKE_INSTALL_PREFIX)
LEMONLOG(CMAKE_BINARY_DIR)
message("")
message("======== LemonLime Build Statistics ========")
message("${LEMON_BUILD_STATS}")
message("============================================")
message("")
