// ==============================================================================
// shortcut_manager.h - Start Menu Shortcut Management
// ==============================================================================
// Description: Helper for creating Start Menu shortcuts required for Toast
//              notifications to work properly on Desktop applications.
// ==============================================================================

#pragma once

#include "error.h"

namespace windows_platform {

/**
 * @brief ShortcutManager - Helper for Start Menu shortcut creation
 * 
 * Desktop applications need a shortcut in the Start Menu with an
 * AppUserModelID for Windows to properly display toast notifications.
 */
class ShortcutManager {
public:
    /**
     * @brief Create or update a Start Menu shortcut for the current application
     * 
     * @param appName Display name for the shortcut (e.g., "My Music Player")
     * @param appId AppUserModelID (same as used in ToastNotifier::create)
     * @param description Optional description text for the shortcut
     * @return ErrorInfo Success or failure information
     * 
     * @note This function should be called once during application startup
     *       or installation. It creates a .lnk file in the user's Start Menu.
     */
    static ErrorInfo createShortcut(
        const char* appName,
        const char* appId,
        const char* description = nullptr
    );
    
    /**
     * @brief Check if a shortcut already exists
     * 
     * @param appName Name of the shortcut to check
     * @return true if shortcut exists, false otherwise
     */
    static bool shortcutExists(const char* appName);
    
    /**
     * @brief Remove the Start Menu shortcut
     * 
     * @param appName Name of the shortcut to remove
     * @return ErrorInfo Success or failure information
     */
    static ErrorInfo removeShortcut(const char* appName);
    
    /**
     * @brief Get the path where the shortcut would be created
     * 
     * @param appName Name of the application
     * @return Path to the shortcut file (empty string on error)
     */
    static std::string getShortcutPath(const char* appName);
};

} // namespace windows_platform
