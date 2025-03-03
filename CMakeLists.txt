cmake_minimum_required(VERSION 3.13...3.23)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(sispop-logging VERSION 1.0.2 LANGUAGES CXX)

set(SISPOP_LOGGING_SOURCE_ROOT "" CACHE PATH "Base path(s) to strip from log message filenames; separate multiple paths with \";\"")
option(SISPOP_LOGGING_FORCE_SUBMODULES "Force use of the bundled fmt/spdlog rather than looking for system packages" OFF)
option(SISPOP_LOGGING_RELEASE_TRACE "Enable trace logging in release builds" OFF)
option(SISPOP_LOGGING_FMT_HEADER_ONLY "Use fmt in header-only mode" OFF)
option(SISPOP_LOGGING_SPDLOG_HEADER_ONLY "Use spdlog in header-only mode" OFF)

if(NOT SISPOP_LOGGING_FORCE_SUBMODULES)
    if(NOT TARGET fmt::fmt)
        find_package(fmt 9.0.0 CONFIG QUIET)
        if(fmt_FOUND)
            message(STATUS "Found fmt ${fmt_VERSION}")
            # Make the target available to the parent project (which is the case if we go by
            # subproject, but isn't for packages we find via find_package).  cmake 3.24+ has a
            # `GLOBAL` flag in the find_package, but we need this to work on older cmakes as well.
            set_target_properties(fmt::fmt PROPERTIES IMPORTED_GLOBAL TRUE)
        else()
            message(STATUS "Did not find suitable fmt; using submodule")
        endif()
    endif()
    if(NOT TARGET spdlog::spdlog)
        find_package(spdlog 1.9.1 CONFIG QUIET)
        if(spdlog_FOUND)
            message(STATUS "Found spdlog ${spdlog_VERSION}")
            # Make available in parent; see above.
            set_target_properties(spdlog::spdlog PROPERTIES IMPORTED_GLOBAL TRUE)
        else()
            message(STATUS "Did not find suitable spdlog; using submodule")
        endif()
    endif()
endif()

set(SISPOP_LOGGING_FMT_TARGET fmt::fmt)
set(SISPOP_LOGGING_SPDLOG_TARGET spdlog::spdlog)

if(NOT TARGET fmt::fmt)
    if(SISPOP_LOGGING_FMT_HEADER_ONLY)
        set(SISPOP_LOGGING_FMT_TARGET fmt::fmt-header-only)
    endif()
    add_subdirectory(fmt)
endif()

if(NOT TARGET spdlog::spdlog)
    if(SISPOP_LOGGING_FMT_HEADER_ONLY)
        set(SPDLOG_FMT_EXTERNAL_HO ON CACHE INTERNAL "")
    else()
        set(SPDLOG_FMT_EXTERNAL ON CACHE INTERNAL "")
    endif()
    if(SISPOP_LOGGING_SPDLOG_HEADER_ONLY)
        set(SISPOP_LOGGING_SPDLOG_TARGET spdlog::spdlog_header_only)
    endif()
    add_subdirectory(spdlog)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(sispop-logging STATIC
    src/catlogger.cpp
    src/level.cpp
    src/log.cpp
    src/type.cpp
)
target_include_directories(sispop-logging PUBLIC include)
target_link_libraries(sispop-logging PUBLIC ${SISPOP_LOGGING_FMT_TARGET} ${SISPOP_LOGGING_SPDLOG_TARGET})
target_compile_features(sispop-logging PUBLIC cxx_std_17)

if(SISPOP_LOGGING_SOURCE_ROOT)
    set(paths)
    foreach(path ${SISPOP_LOGGING_SOURCE_ROOT})
        string(REGEX REPLACE "([\\\"])" "\\\\\\1" path_escaped "${path}")
        list(APPEND paths "\"${path_escaped}\"")
        message(STATUS "Stripping source root ${path} (${path_escaped}) from log paths")
    endforeach()
    list(LENGTH paths pathlen)
    list(JOIN paths ", " paths)
    target_compile_definitions(sispop-logging PUBLIC
        SISPOP_LOGGING_SOURCE_ROOTS=${paths}
        SISPOP_LOGGING_SOURCE_ROOTS_LEN=${pathlen}
    )
else()
    message(STATUS "Source root log path stripping disabled")
endif()

if (SISPOP_LOGGING_RELEASE_TRACE)
    target_compile_definitions(sispop-logging PUBLIC SISPOP_LOGGING_RELEASE_TRACE)
endif()

add_library(sispop::logging ALIAS sispop-logging)
