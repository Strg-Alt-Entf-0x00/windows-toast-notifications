// ==============================================================================
// windows-toast-notifications-0.9.0 - Type Definitions
// ==============================================================================
// Description: Enums, structs, and type definitions for toast notifications
// License:     MIT
// Created:     2026-06-22
// ==============================================================================

#pragma once

#include <cstdint>
#include <chrono>

namespace windows_platform {

// ==============================================================================
// Enums
// ==============================================================================

/**
 * @brief Toast notification duration
 * 
 * Controls how long the toast stays on screen before auto-dismissing.
 */
enum class ToastDuration : uint8_t {
    Short = 0,  ///< 5 seconds (default)
    Long = 1    ///< 25 seconds
};

/**
 * @brief Toast notification scenario
 * 
 * Affects toast behavior (sound, persistence, priority).
 */
enum class ToastScenario : uint8_t {
    Default = 0,        ///< Standard notification
    Reminder = 1,       ///< Reminder (stays until dismissed)
    Alarm = 2,          ///< Alarm (loops sound, stays until dismissed)
    IncomingCall = 3    ///< Incoming call (full-screen on lock screen)
};

/**
 * @brief Toast audio mode
 * 
 * Controls notification sound behavior.
 */
enum class ToastAudio : uint8_t {
    Default = 0,   ///< Default notification sound
    Silent = 1,    ///< No sound
    Loop = 2       ///< Loop sound (use with Alarm scenario)
};

/**
 * @brief Image position in toast
 * 
 * Controls where the image appears in the toast layout.
 */
enum class ToastImagePlacement : uint8_t {
    Inline = 0,    ///< Inside toast body (default)
    AppLogo = 1,   ///< Replace app icon
    Hero = 2       ///< Large header image
};

// ==============================================================================
// Structs
// ==============================================================================

/**
 * @brief Toast notification content
 * 
 * Defines the text and visual content of a toast notification.
 */
struct ToastContent {
    const char* title = nullptr;         ///< Title text (required)
    const char* body = nullptr;          ///< Body text (optional)
    const char* imagePath = nullptr;     ///< Image path: file:/// or http:// (optional)
    
    ToastDuration duration = ToastDuration::Short;
    ToastScenario scenario = ToastScenario::Default;
    ToastAudio audio = ToastAudio::Default;
    ToastImagePlacement imagePlacement = ToastImagePlacement::Inline;
    
    /**
     * @brief Attribution text (small text at bottom)
     * Example: "via SinfonAI"
     */
    const char* attribution = nullptr;
};

/**
 * @brief Toast action button
 * 
 * Defines a clickable button on the toast notification.
 */
struct ToastButton {
    const char* label = nullptr;         ///< Button text (required)
    const char* arguments = nullptr;     ///< Launch arguments when clicked (required)
    
    /**
     * @brief Button image icon (optional)
     * Path to icon file (file:/// or http://)
     */
    const char* iconPath = nullptr;
};

/**
 * @brief Toast input field (text box)
 * 
 * Allows user to enter text in the toast (e.g., quick reply).
 */
struct ToastInput {
    const char* id = nullptr;            ///< Input field ID (for retrieval)
    const char* placeholder = nullptr;   ///< Placeholder text
    const char* defaultValue = nullptr;  ///< Default text value
};

/**
 * @brief Toast progress bar
 * 
 * Shows progress indicator in toast (e.g., download/upload progress).
 */
struct ToastProgress {
    const char* title = nullptr;         ///< Progress title (optional)
    const char* status = nullptr;        ///< Status text (optional)
    double value = 0.0;                  ///< Progress value (0.0 - 1.0)
    const char* valueString = nullptr;   ///< Custom value text (e.g., "5/10")
};

} // namespace windows_platform
