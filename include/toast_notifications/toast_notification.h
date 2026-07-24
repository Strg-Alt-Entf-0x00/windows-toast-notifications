// ==============================================================================
// windows-toast-notifications-0.9.0 - Main Public API
// ==============================================================================
// Description: Production-grade Windows Toast Notifications (Action Center)
// License:     MIT
// Created:     2026-06-22
// ==============================================================================
// 
// This library provides framework-agnostic C++20 interface to Windows
// Toast Notifications (Windows.UI.Notifications API).
// 
// Features:
//   - Modern Windows 10/11 toast notifications
//   - Action Center integration (persistent notifications)
//   - Action buttons (up to 5 buttons)
//   - Inline images (album art, icons, etc.)
//   - Text input fields (quick reply)
//   - Progress bars
//   - Custom sounds
//   - Click activation callbacks
// 
// Requirements:
//   - Windows 10 Build 10240+ (2015)
//   - C++20 compiler
//   - MSVC 2022+ or Clang 14+
// 
// Example:
//   #include <toast_notifications/toast_notification.h>
//   
//   auto toast = windows_platform::ToastNotification::create("MyApp.ID");
//   
//   windows_platform::ToastContent content;
//   content.title = "Now Playing";
//   content.body = "Hotel California - Eagles";
//   content.imagePath = "file:///C:/Music/cover.jpg";
//   
//   toast->show(content);
// 
// ==============================================================================

#pragma once

#include "error.h"
#include "types.h"

#include <functional>
#include <memory>

namespace windows_platform {

// ==============================================================================
// ToastNotifier Class
// ==============================================================================
/**
 * @brief Windows Toast Notification Manager
 * 
 * Provides access to Windows Toast Notifications (Action Center).
 * Shows modern Windows 10/11 style notifications instead of old balloon tips.
 * 
 * Thread Safety:
 *   - All public methods are thread-safe
 *   - Callbacks are invoked on the application thread
 * 
 * Lifecycle:
 *   - Create via factory method: create()
 *   - Automatically cleaned up via RAII (unique_ptr)
 *   - Must keep instance alive while toasts are active
 * 
 * Error Handling:
 *   - All methods return ErrorInfo (no exceptions in public API)
 *   - Check error.isSuccess() before proceeding
 * 
 * Example:
 * @code
 *   auto toast = ToastNotifier::create("SinfonAI.MusicPlayer");
 *   if (!toast) {
 *       fprintf(stderr, "Failed to create ToastNotifier\n");
 *       return;
 *   }
 *   
 *   ToastContent content;
 *   content.title = "Now Playing";
 *   content.body = "Test Song";
 *   
 *   auto error = toast->show(content);
 *   if (!error.isSuccess()) {
 *       fprintf(stderr, "Error: %s\n", error.message);
 *   }
 * @endcode
 */
class ToastNotifier {
public:
    // ==========================================================================
    // Factory Method
    // ==========================================================================
    /**
     * @brief Create a new ToastNotifier instance
     * 
     * @param appId Application User Model ID (AppUserModelID)
     *              Example: "MyCompany.MyApp" or "SinfonAI.MusicPlayer"
     *              Used for grouping toasts in Action Center.
     * 
     * @return std::unique_ptr<ToastNotifier> on success, nullptr on failure
     * 
     * Failure reasons:
     *   - Unsupported OS version (< Windows 10 Build 10240)
     *   - AppUserModelID registration failed
     *   - WinRT initialization failed
     *   - Out of memory
     * 
     * Note: Desktop apps require AppUserModelID registration.
     *       This is handled automatically by the library.
     * 
     * Example:
     * @code
     *   auto toast = ToastNotifier::create("SinfonAI.MusicPlayer");
     *   if (!toast) {
     *       // Handle error
     *   }
     * @endcode
     */
    [[nodiscard]] static std::unique_ptr<ToastNotifier> create(
        const char* appId = "DefaultApp.Notifications"
    );
    
    // ==========================================================================
    // Show Toast Notifications
    // ==========================================================================
    /**
     * @brief Show a simple toast notification
     * 
     * Displays a toast with title, body, and optional image.
     * Toast appears in corner of screen and in Action Center.
     * 
     * @param content Toast content (title, body, image, etc.)
     * @return ErrorInfo (Success or error details)
     * 
     * Example:
     * @code
     *   ToastContent content;
     *   content.title = "Now Playing";
     *   content.body = "Hotel California\nEagles";
     *   content.imagePath = "file:///C:/Music/cover.jpg";
     *   content.duration = ToastDuration::Short;
     *   
     *   toast->show(content);
     * @endcode
     */
    [[nodiscard]] ErrorInfo show(const ToastContent& content) noexcept;
    
    /**
     * @brief Show toast with action buttons
     * 
     * Displays a toast with clickable buttons.
     * Use onActivated() to handle button clicks.
     * 
     * @param content Toast content
     * @param buttons Array of button definitions
     * @param buttonCount Number of buttons (max 5)
     * @return ErrorInfo (Success or error details)
     * 
     * Example:
     * @code
     *   ToastButton buttons[] = {
     *       { "▶️ Play", "action=play" },
     *       { "⏭️ Next", "action=next" }
     *   };
     *   
     *   toast->showWithButtons(content, buttons, 2);
     * @endcode
     */
    [[nodiscard]] ErrorInfo showWithButtons(
        const ToastContent& content,
        const ToastButton* buttons,
        size_t buttonCount
    ) noexcept;
    
    /**
     * @brief Show toast with text input field
     * 
     * Displays a toast with a text input box (e.g., quick reply).
     * Use onActivated() to retrieve input text.
     * 
     * @param content Toast content
     * @param input Input field definition
     * @param buttons Optional buttons (can be nullptr)
     * @param buttonCount Number of buttons
     * @return ErrorInfo (Success or error details)
     * 
     * Example:
     * @code
     *   ToastInput input;
     *   input.id = "replyBox";
     *   input.placeholder = "Type your reply...";
     *   
     *   ToastButton button = { "Send", "action=send" };
     *   
     *   toast->showWithInput(content, input, &button, 1);
     * @endcode
     */
    [[nodiscard]] ErrorInfo showWithInput(
        const ToastContent& content,
        const ToastInput& input,
        const ToastButton* buttons = nullptr,
        size_t buttonCount = 0
    ) noexcept;
    
    /**
     * @brief Show toast with progress bar
     * 
     * Displays a toast with a progress indicator.
     * Use updateProgress() to update the progress bar.
     * 
     * @param content Toast content
     * @param progress Progress bar definition
     * @param tag Unique tag for updating progress (required)
     * @return ErrorInfo (Success or error details)
     * 
     * Example:
     * @code
     *   ToastProgress progress;
     *   progress.title = "Downloading";
     *   progress.value = 0.5;  // 50%
     *   progress.valueString = "5/10 files";
     *   
     *   toast->showWithProgress(content, progress, "download123");
     * @endcode
     */
    [[nodiscard]] ErrorInfo showWithProgress(
        const ToastContent& content,
        const ToastProgress& progress,
        const char* tag
    ) noexcept;
    
    // ==========================================================================
    // Update & Manage Toasts
    // ==========================================================================
    /**
     * @brief Update progress bar in existing toast
     * 
     * Updates the progress value without re-showing the toast.
     * 
     * @param tag Unique tag from showWithProgress()
     * @param progress Updated progress data
     * @return ErrorInfo (Success or error details)
     * 
     * Example:
     * @code
     *   ToastProgress updated;
     *   updated.value = 0.8;  // 80%
     *   updated.valueString = "8/10 files";
     *   
     *   toast->updateProgress("download123", updated);
     * @endcode
     */
    [[nodiscard]] ErrorInfo updateProgress(
        const char* tag,
        const ToastProgress& progress
    ) noexcept;
    
    /**
     * @brief Clear all toasts from Action Center
     * 
     * Removes all notifications created by this app from Action Center.
     * 
     * @return ErrorInfo (Success or error details)
     */
    [[nodiscard]] ErrorInfo clearAll() noexcept;
    
    /**
     * @brief Clear specific toast by tag
     * 
     * Removes a specific notification from Action Center.
     * 
     * @param tag Toast tag (from showWithProgress or custom)
     * @return ErrorInfo (Success or error details)
     */
    [[nodiscard]] ErrorInfo clearByTag(const char* tag) noexcept;
    
    // ==========================================================================
    // Activation Callbacks
    // ==========================================================================
    /**
     * @brief Register callback for toast activation
     * 
     * Called when user clicks on toast or a button.
     * 
     * @param callback Function to call with activation arguments
     * 
     * Callback receives:
     *   - arguments: Button arguments or empty string for toast body click
     *   - userInput: Text from input field (if any)
     * 
     * Example:
     * @code
     *   toast->onActivated([](const char* args, const char* input) {
     *       if (strcmp(args, "action=play") == 0) {
     *           printf("User clicked Play!\n");
     *       }
     *       if (input && strlen(input) > 0) {
     *           printf("User typed: %s\n", input);
     *       }
     *   });
     * @endcode
     */
    void onActivated(std::function<void(const char* arguments, const char* userInput)> callback) noexcept;
    
    /**
     * @brief Register callback for toast dismissal
     * 
     * Called when toast is dismissed (user click X or timeout).
     * 
     * @param callback Function to call on dismissal
     * 
     * Example:
     * @code
     *   toast->onDismissed([]() {
     *       printf("Toast was dismissed\n");
     *   });
     * @endcode
     */
    void onDismissed(std::function<void()> callback) noexcept;
    
    // ==========================================================================
    // Lifecycle
    // ==========================================================================
    /**
     * @brief Destructor (cleanup WinRT resources)
     */
    ~ToastNotifier();
    
    // Delete copy/move (PIMPL with unique ownership)
    ToastNotifier(const ToastNotifier&) = delete;
    ToastNotifier& operator=(const ToastNotifier&) = delete;
    ToastNotifier(ToastNotifier&&) = delete;
    ToastNotifier& operator=(ToastNotifier&&) = delete;

private:
    // Constructor is private (use factory create())
    ToastNotifier();
    
    // PIMPL: Hide WinRT implementation details
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace windows_platform
