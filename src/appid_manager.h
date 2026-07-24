// ==============================================================================
// appid_manager.h - AppUserModelID Registration for Desktop Apps
// ==============================================================================

#pragma once

#include <string>

namespace windows_platform {

// ==============================================================================
// AppIdManager - Desktop App Registration
// ==============================================================================
/**
 * @brief Manages AppUserModelID registration for Desktop applications
 * 
 * Desktop apps need to register an AppUserModelID to show toast notifications.
 * This class handles the registration process automatically.
 * 
 * Background:
 *   - UWP apps have AppUserModelID automatically
 *   - Desktop apps (Win32, Qt, WPF) need manual registration
 *   - Registration is per-process (one-time)
 */
class AppIdManager {
public:
    /**
     * @brief Check if OS supports toast notifications
     * @return true if Windows 10 Build 10240+ (2015)
     */
    static bool isSupported();
    
    /**
     * @brief Register AppUserModelID for current process
     * 
     * Must be called before showing any toasts.
     * Safe to call multiple times (idempotent).
     * 
     * @param appId Application User Model ID (e.g., "SinfonAI.MusicPlayer")
     * @return true if registration succeeded
     */
    static bool registerAppId(const char* appId);
    
    /**
     * @brief Get currently registered AppUserModelID
     * @return Registered AppID or empty string if not registered
     */
    static std::string getCurrentAppId();

private:
    AppIdManager() = delete;  // Static-only class
};

} // namespace windows_platform
