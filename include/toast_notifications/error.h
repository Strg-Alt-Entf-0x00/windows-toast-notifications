// ==============================================================================
// windows-toast-notifications-0.9.0 - Error Handling
// ==============================================================================
// Description: Exception-free error reporting using ErrorInfo pattern
// License:     MIT
// Created:     2026-06-22
// ==============================================================================

#pragma once

#include <string>
#include <string_view>

namespace windows_platform {

// ==============================================================================
// ErrorInfo - Exception-Free Error Reporting
// ==============================================================================
/**
 * @brief Error information container
 * 
 * Provides exception-free error reporting for all public API methods.
 * Prefer checking isSuccess() over throwing exceptions.
 * 
 * Thread Safety: Value type, safe to copy across threads
 * 
 * Example:
 * @code
 *   auto error = toast->show(content);
 *   if (!error.isSuccess()) {
 *       fprintf(stderr, "Error: %s\n", error.message);
 *   }
 * @endcode
 */
struct ErrorInfo {
    int code;                    ///< Error code (0 = success)
    const char* message;         ///< Human-readable error message
    
    /**
     * @brief Check if operation succeeded
     * @return true if no error occurred
     */
    [[nodiscard]] constexpr bool isSuccess() const noexcept {
        return code == 0;
    }
    
    /**
     * @brief Check if operation failed
     * @return true if error occurred
     */
    [[nodiscard]] constexpr bool isError() const noexcept {
        return code != 0;
    }
    
    /**
     * @brief Implicit conversion to bool (true = success)
     * 
     * Allows usage: if (error) { // success  }
     */
    [[nodiscard]] constexpr explicit operator bool() const noexcept {
        return isSuccess();
    }
};

// ==============================================================================
// Predefined Error Codes
// ==============================================================================
namespace ErrorCode {
    constexpr int Success = 0;
    constexpr int NotInitialized = 1;
    constexpr int InitializationFailed = 2;
    constexpr int InvalidParameter = 3;
    constexpr int InvalidImageFormat = 4;
    constexpr int UnsupportedOS = 5;
    constexpr int ToastFailed = 6;
    constexpr int AppIdRegistrationFailed = 7;
    constexpr int XmlGenerationFailed = 8;
    constexpr int UnknownError = 99;
} // namespace ErrorCode

// ==============================================================================
// Predefined Error Messages
// ==============================================================================
namespace Errors {
    constexpr ErrorInfo Success{ ErrorCode::Success, "Success" };
    constexpr ErrorInfo NotInitialized{ ErrorCode::NotInitialized, "ToastNotification not initialized" };
    constexpr ErrorInfo InitializationFailed{ ErrorCode::InitializationFailed, "Failed to initialize ToastNotificationManager" };
    constexpr ErrorInfo InvalidParameter{ ErrorCode::InvalidParameter, "Invalid parameter provided" };
    constexpr ErrorInfo InvalidImageFormat{ ErrorCode::InvalidImageFormat, "Image format not supported (use PNG/JPEG/BMP)" };
    constexpr ErrorInfo UnsupportedOS{ ErrorCode::UnsupportedOS, "Requires Windows 10 Build 10240 or later" };
    constexpr ErrorInfo ToastFailed{ ErrorCode::ToastFailed, "Failed to show toast notification" };
    constexpr ErrorInfo AppIdRegistrationFailed{ ErrorCode::AppIdRegistrationFailed, "Failed to register AppUserModelID" };
    constexpr ErrorInfo XmlGenerationFailed{ ErrorCode::XmlGenerationFailed, "Failed to generate toast XML" };
    constexpr ErrorInfo UnknownError{ ErrorCode::UnknownError, "An unknown error occurred" };
} // namespace Errors

} // namespace windows_platform
