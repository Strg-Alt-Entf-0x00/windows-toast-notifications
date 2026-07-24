// ==============================================================================
// types_v2.h - Enhanced Toast Notification Types (World-Class Edition)
// ==============================================================================
// Description: Extended type definitions inspired by WinToast + modern C++20
// Features:    Hero images, audio variety, expiration, circular crops
// ==============================================================================

#pragma once

#include <cstdint>

namespace windows_platform {

// ==============================================================================
// Enhanced Enumerations
// ==============================================================================

/**
 * @brief Toast notification duration
 */
enum class ToastDuration : uint8_t {
    System = 0,  ///< System default setting
    Short = 1,   ///< ~5 seconds
    Long = 2     ///< ~25 seconds
};

/**
 * @brief Extended audio options (inspired by WinToast)
 */
enum class ToastAudio : uint8_t {
    Default = 0,     ///< Default notification sound (plays once)
    Silent = 1,      ///< No sound
    
    // Looping sounds (for alarms/reminders)
    LoopDefault = 10,///< Loop default sound
    LoopIM = 11,     ///< Loop IM sound
    LoopMail = 12,   ///< Loop mail sound
    LoopReminder = 13, ///< Loop reminder sound
    LoopSMS = 14,    ///< Loop SMS sound
    
    // Alarm sounds
    Alarm = 20,      ///< Default alarm
    Alarm2 = 21,     ///< Alarm variation 2
    Alarm3 = 22,     ///< Alarm variation 3
    Alarm4 = 23,     ///< Alarm variation 4
    Alarm5 = 24,     ///< Alarm variation 5
    Alarm6 = 25,     ///< Alarm variation 6
    Alarm7 = 26,     ///< Alarm variation 7
    Alarm8 = 27,     ///< Alarm variation 8
    Alarm9 = 28,     ///< Alarm variation 9
    Alarm10 = 29,    ///< Alarm variation 10
    
    // Call sounds
    Call = 30,       ///< Incoming call sound
    Call2 = 31,      ///< Call variation 2
    Call3 = 32,      ///< Call variation 3
    Call4 = 33,      ///< Call variation 4
    Call5 = 34,      ///< Call variation 5
    Call6 = 35,      ///< Call variation 6
    Call7 = 36,      ///< Call variation 7
    Call8 = 37,      ///< Call variation 8
    Call9 = 38,      ///< Call variation 9
    Call10 = 39,     ///< Call variation 10
    
    // Standard notification sounds
    IM = 40,         ///< Instant message sound
    Mail = 41,       ///< Mail notification sound
    Reminder = 42,   ///< Reminder sound
    SMS = 43         ///< SMS notification sound
};

/**
 * @brief Toast notification scenarios
 */
enum class ToastScenario : uint8_t {
    Default = 0,        ///< Normal notification
    Reminder = 1,       ///< Persistent until dismissed
    Alarm = 2,          ///< Persistent with alarm sound
    IncomingCall = 3,   ///< Full-screen on lock screen
    Urgent = 4          ///< Windows 11: Breaks through Focus Assist
};

/**
 * @brief Image placement options
 */
enum class ToastImagePlacement : uint8_t {
    Inline = 0,    ///< Inside toast body (default)
    AppLogo = 1,   ///< Replace app icon (square)
    Hero = 2       ///< Large banner image at top
};

/**
 * @brief Image crop style (for profile pictures, etc.)
 */
enum class ToastImageCrop : uint8_t {
    Default = 0,   ///< Rectangular (default)
    Circle = 1     ///< Circular crop (recommended for profile pics)
};

// ==============================================================================
// Enhanced Structures
// ==============================================================================

/**
 * @brief World-class toast notification content
 * 
 * Combines features from WinToast, Windows SDK, and modern best practices.
 */
struct ToastContentV2 {
    // === Basic Content ===
    const char* title = nullptr;                    ///< Title text (REQUIRED)
    const char* body = nullptr;                     ///< Body text (optional)
    const char* attribution = nullptr;              ///< Attribution text (e.g., "via SMS")
    
    // === Images ===
    const char* imagePath = nullptr;                ///< Standard image (file:/// or http://)
    const char* heroImagePath = nullptr;            ///< Hero image - large banner at top
    ToastImagePlacement imagePlacement = ToastImagePlacement::Inline;
    ToastImageCrop imageCrop = ToastImageCrop::Default;
    bool heroImageInline = false;                   ///< Display hero below text instead of above
    
    // === Behavior ===
    ToastDuration duration = ToastDuration::Short;
    ToastScenario scenario = ToastScenario::Default;
    ToastAudio audio = ToastAudio::Default;
    
    // === Advanced ===
    int expirationMs = 0;                          ///< Auto-dismiss after X ms (0 = system default)
    const char* tag = nullptr;                     ///< Unique tag for updating/removing toast
    const char* group = nullptr;                   ///< Group tag for related toasts
    bool suppressPopup = false;                    ///< Skip popup, only show in Action Center
};

/**
 * @brief Enhanced button with tooltip support
 */
struct ToastButtonV2 {
    const char* label = nullptr;                   ///< Button text (REQUIRED)
    const char* arguments = nullptr;               ///< Launch arguments (REQUIRED)
    const char* iconPath = nullptr;                ///< Button icon (optional)
    const char* tooltip = nullptr;                 ///< Tooltip text (optional, Windows 11+)
    bool activateInBackground = false;             ///< Don't bring app to foreground
};

/**
 * @brief Enhanced input field with title
 */
struct ToastInputV2 {
    const char* id = nullptr;                      ///< Input ID (REQUIRED)
    const char* title = nullptr;                   ///< Input title/label
    const char* placeholder = nullptr;             ///< Placeholder text
    const char* defaultValue = nullptr;            ///< Default value
};

/**
 * @brief Enhanced progress bar with indeterminate support
 */
struct ToastProgressV2 {
    const char* title = nullptr;                   ///< Progress title
    const char* status = nullptr;                  ///< Status text
    double value = 0.0;                           ///< Progress (0.0-1.0, or -1 for indeterminate)
    const char* valueString = nullptr;            ///< Custom display (e.g., "5/10 files")
};

// ==============================================================================
// Backwards Compatibility Aliases
// ==============================================================================

using ToastContent = ToastContentV2;
using ToastButton = ToastButtonV2;
using ToastInput = ToastInputV2;
using ToastProgress = ToastProgressV2;

} // namespace windows_platform
