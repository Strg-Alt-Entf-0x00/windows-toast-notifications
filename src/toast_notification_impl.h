// ==============================================================================
// toast_notification_impl.h - PIMPL Implementation (WinRT)
// ==============================================================================

#pragma once

#include <toast_notifications/toast_notification.h>
#include <functional>
#include <string>
#include <any>

namespace windows_platform {

// Forward declare WinRT types to avoid namespace pollution
// Note: We don't use 'using namespace winrt' here to avoid conflicts

// ==============================================================================
// ToastNotifier::Impl - Private Implementation
// ==============================================================================

struct ToastNotifier::Impl {
    // Initialization
    bool initialize(const char* appId);
    
    // Show methods
    ErrorInfo show(const ToastContent& content);
    ErrorInfo showWithButtons(const ToastContent& content, const ToastButton* buttons, size_t buttonCount);
    ErrorInfo showWithInput(const ToastContent& content, const ToastInput& input, const ToastButton* buttons, size_t buttonCount);
    ErrorInfo showWithProgress(const ToastContent& content, const ToastProgress& progress, const char* tag);
    
    // Update methods
    ErrorInfo updateProgress(const char* tag, const ToastProgress& progress);
    ErrorInfo clearAll();
    ErrorInfo clearByTag(const char* tag);
    
    // Callbacks
    void onActivated(std::function<void(const char*, const char*)> callback);
    void onDismissed(std::function<void()> callback);
    
    // Helper methods - XML generation
    std::wstring generateToastXml(const ToastContent& content, const ToastButton* buttons, size_t buttonCount);
    std::wstring generateToastXmlWithInput(const ToastContent& content, const ToastInput& input, const ToastButton* buttons, size_t buttonCount);
    std::wstring generateToastXmlWithProgress(const ToastContent& content, const ToastProgress& progress);
    
    // Helper methods - String conversion
    std::wstring toWide(const char* str);
    std::string toNarrow(const wchar_t* wstr);
    
    // State
    std::string appId_;
    bool initialized_ = false;
    
    // Callbacks
    std::function<void(const char*, const char*)> activatedCallback_;
    std::function<void()> dismissedCallback_;
    
    // WinRT objects (stored directly in std::any)
    // We use std::any to avoid WinRT types in header
    std::any toastNotifier_;  // Will hold winrt::Windows::UI::Notifications::ToastNotifier (by value)
};

} // namespace windows_platform
