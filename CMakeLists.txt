cmake_minimum_required(VERSION 3.20)
project(windows-toast-notifications VERSION 0.9.0 LANGUAGES CXX)

# ==============================================================================
# windows-toast-notifications-0.9.0
# Windows Toast Notifications Library
# ==============================================================================

# C++20 Required
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# MSVC-specific flags (match windows-media-session pattern)
if(MSVC)
    # Warning level 4, treat warnings as errors
    add_compile_options(/W4 /WX)
    
    # UTF-8 encoding
    add_compile_options(/utf-8)
    
    # C++20 standard
    add_compile_options(/std:c++20)
    
    # Enable C++20 Coroutines (required for WinRT)
    add_compile_options(/await)
    
    # Enable __cplusplus macro
    add_compile_options(/Zc:__cplusplus)
    
    # Optimization for Release
    add_compile_options($<$<CONFIG:Release>:/O2>)
    add_compile_options($<$<CONFIG:Release>:/Ob2>)
endif()

# ==============================================================================
# Library Target
# ==============================================================================

add_library(windows-toast-notifications STATIC
    # Public headers
    include/toast_notifications/toast_notification.h
    include/toast_notifications/types.h
    include/toast_notifications/error.h
    include/toast_notifications/shortcut_manager.h
    
    # Implementation
    src/toast_notification.cpp
    src/toast_notification_impl.h
    src/toast_notification_impl.cpp
    src/appid_manager.h
    src/appid_manager.cpp
    src/shortcut_manager.cpp
)

# Include directories
target_include_directories(windows-toast-notifications
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Windows libraries
target_link_libraries(windows-toast-notifications
    PRIVATE
        # Windows Runtime (WinRT)
        windowsapp
        
        # Win32 APIs
        user32
        ole32
        shell32
)

# Set target properties
set_target_properties(windows-toast-notifications PROPERTIES
    OUTPUT_NAME "windows-toast-notifications"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Release"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Release"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Release"
)

# ==============================================================================
# Examples (Optional)
# ==============================================================================

option(BUILD_EXAMPLES "Build example programs" ON)
option(BUILD_TESTS "Build test suite" ON)

if(BUILD_EXAMPLES)
    # Basic toast example
    add_executable(basic_toast
        examples/basic_toast.cpp
    )
    
    target_link_libraries(basic_toast
        PRIVATE
            windows-toast-notifications
    )
    
    set_target_properties(basic_toast PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
    )
    
    # Advanced toast example (with buttons & image)
    add_executable(advanced_toast
        examples/advanced_toast.cpp
    )
    
    target_link_libraries(advanced_toast
        PRIVATE
            windows-toast-notifications
    )
    
    set_target_properties(advanced_toast PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/examples"
    )
endif()

# ==============================================================================
# Tests (Optional)
# ==============================================================================

if(BUILD_TESTS)
    # Visual test (confirmation that toasts appear)
    add_executable(visual_test
        tests/visual_test.cpp
    )
    
    target_link_libraries(visual_test
        PRIVATE
            windows-toast-notifications
    )
    
    set_target_properties(visual_test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
    )
    
    # Persistent test (toasts that stay in Action Center)
    add_executable(persistent_test
        tests/persistent_test.cpp
    )
    
    target_link_libraries(persistent_test
        PRIVATE
            windows-toast-notifications
    )
    
    set_target_properties(persistent_test PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
    )
    
    # Test with shortcut creation
    add_executable(test_with_shortcut
        tests/test_with_shortcut.cpp
    )
    
    target_link_libraries(test_with_shortcut
        PRIVATE
            windows-toast-notifications
            ole32
            shell32
            propsys
    )
    
    set_target_properties(test_with_shortcut PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
    )
    
    # Unit test suite (automated, no user interaction)
    add_executable(unit_tests
        tests/test_unit.cpp
    )
    
    target_link_libraries(unit_tests
        PRIVATE
            windows-toast-notifications
    )
    
    set_target_properties(unit_tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
    )
    
    # Diagnostic test (for debugging library issues)
    add_executable(test_diagnose
        tests/test_diagnose.cpp
    )
    
    target_link_libraries(test_diagnose
        PRIVATE
            windows-toast-notifications
    )
    
    set_target_properties(test_diagnose PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
    )
    
    # Direct WinRT test (bypass library, test WinRT directly)
    add_executable(test_winrt_direct
        tests/test_winrt_direct.cpp
    )
    
    target_link_libraries(test_winrt_direct
        PRIVATE
            windowsapp
            user32
            ole32
            shell32
    )
    
    set_target_properties(test_winrt_direct PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
    )
    
    # Factory debug test
    add_executable(test_debug_factory
        tests/test_debug_factory.cpp
        src/appid_manager.cpp
    )
    
    target_include_directories(test_debug_factory
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
    
    target_link_libraries(test_debug_factory
        PRIVATE
            user32
            ole32
            shell32
    )
    
    set_target_properties(test_debug_factory PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
    )
    
    # Notification settings check
    add_executable(test_notifications_settings
        tests/test_notifications_settings.cpp
    )
    
    target_link_libraries(test_notifications_settings
        PRIVATE
            windowsapp
            user32
            ole32
            shell32
    )
    
    set_target_properties(test_notifications_settings PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
    )
endif()

# ==============================================================================
# Installation
# ==============================================================================

install(TARGETS windows-toast-notifications
    EXPORT windows-toast-notifications-targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/toast_notifications
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# ==============================================================================
# Status Messages
# ==============================================================================

message(STATUS "")
message(STATUS "========================================")
message(STATUS "windows-toast-notifications Configuration")
message(STATUS "========================================")
message(STATUS "Version:        ${PROJECT_VERSION}")
message(STATUS "Build Type:     ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard:   C++${CMAKE_CXX_STANDARD}")
if(MSVC)
    message(STATUS "Compiler:       MSVC ${MSVC_VERSION}")
else()
    message(STATUS "Compiler:       ${CMAKE_CXX_COMPILER_ID}")
endif()
message(STATUS "Build Examples: ${BUILD_EXAMPLES}")
message(STATUS "Build Tests:    ${BUILD_TESTS}")
message(STATUS "========================================")
message(STATUS "")
