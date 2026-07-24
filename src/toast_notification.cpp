// ==============================================================================
// toast_notification.cpp - Factory and Public API Implementation
// ==============================================================================

#include <toast_notifications/toast_notification.h>
#include "toast_notification_impl.h"
#include "appid_manager.h"

#include <memory>

namespace windows_platform {

// ==============================================================================
// Factory Method
// ==============================================================================

std::unique_ptr<ToastNotifier> ToastNotifier::create(const char* appId) {
    // Check Windows version (requires Windows 10 Build 10240+)
    if (!AppIdManager::isSupported()) {
        return nullptr;
    }
    
    // Register AppUserModelID (required for Desktop apps)
    if (!AppIdManager::registerAppId(appId)) {
        return nullptr;
    }
    
    // Create instance
    auto instance = std::unique_ptr<ToastNotifier>(new ToastNotifier());
    
    // Initialize implementation
    if (!instance->impl_->initialize(appId)) {
        return nullptr;
    }
    
    return instance;
}

// ==============================================================================
// Constructor / Destructor
// ==============================================================================

ToastNotifier::ToastNotifier()
    : impl_(std::make_unique<Impl>())
{
}

ToastNotifier::~ToastNotifier() = default;

// ==============================================================================
// Show Toast Notifications
// ==============================================================================

ErrorInfo ToastNotifier::show(const ToastContent& content) noexcept {
    if (!impl_) {
        return Errors::NotInitialized;
    }
    return impl_->show(content);
}

ErrorInfo ToastNotifier::showWithButtons(
    const ToastContent& content,
    const ToastButton* buttons,
    size_t buttonCount
) noexcept {
    if (!impl_) {
        return Errors::NotInitialized;
    }
    return impl_->showWithButtons(content, buttons, buttonCount);
}

ErrorInfo ToastNotifier::showWithInput(
    const ToastContent& content,
    const ToastInput& input,
    const ToastButton* buttons,
    size_t buttonCount
) noexcept {
    if (!impl_) {
        return Errors::NotInitialized;
    }
    return impl_->showWithInput(content, input, buttons, buttonCount);
}

ErrorInfo ToastNotifier::showWithProgress(
    const ToastContent& content,
    const ToastProgress& progress,
    const char* tag
) noexcept {
    if (!impl_) {
        return Errors::NotInitialized;
    }
    return impl_->showWithProgress(content, progress, tag);
}

// ==============================================================================
// Update & Manage Toasts
// ==============================================================================

ErrorInfo ToastNotifier::updateProgress(
    const char* tag,
    const ToastProgress& progress
) noexcept {
    if (!impl_) {
        return Errors::NotInitialized;
    }
    return impl_->updateProgress(tag, progress);
}

ErrorInfo ToastNotifier::clearAll() noexcept {
    if (!impl_) {
        return Errors::NotInitialized;
    }
    return impl_->clearAll();
}

ErrorInfo ToastNotifier::clearByTag(const char* tag) noexcept {
    if (!impl_) {
        return Errors::NotInitialized;
    }
    return impl_->clearByTag(tag);
}

// ==============================================================================
// Activation Callbacks
// ==============================================================================

void ToastNotifier::onActivated(
    std::function<void(const char*, const char*)> callback
) noexcept {
    if (impl_) {
        impl_->onActivated(std::move(callback));
    }
}

void ToastNotifier::onDismissed(std::function<void()> callback) noexcept {
    if (impl_) {
        impl_->onDismissed(std::move(callback));
    }
}

} // namespace windows_platform
