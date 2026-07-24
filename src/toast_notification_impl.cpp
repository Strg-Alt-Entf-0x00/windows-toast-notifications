// ==============================================================================
// toast_notification_impl.cpp - WinRT Implementation
// ==============================================================================

#include "toast_notification_impl.h"
#include "appid_manager.h"

#include <Windows.h>
#include <sstream>
#include <any>

// WinRT includes
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Notifications.h>
#include <winrt/Windows.Data.Xml.Dom.h>

// Use global winrt namespace (not windows_platform::winrt)
using namespace winrt::Windows::UI::Notifications;
using namespace winrt::Windows::Data::Xml::Dom;
using namespace winrt::Windows::Foundation;

namespace windows_platform {

// ==============================================================================
// Helper Functions
// ==============================================================================

// Get ToastNotifier from std::any (WinRT type, not our class!)
static winrt::Windows::UI::Notifications::ToastNotifier& getNotifier(std::any& notifierAny) {
    return std::any_cast<winrt::Windows::UI::Notifications::ToastNotifier&>(notifierAny);
}

// ==============================================================================
// Initialization
// ==============================================================================

bool ToastNotifier::Impl::initialize(const char* appId) {
    if (!appId || strlen(appId) == 0) {
        return false;
    }
    
    appId_ = appId;
    
    try {
        // Initialize WinRT apartment (ignore errors if already initialized)
        try {
            winrt::init_apartment(winrt::apartment_type::multi_threaded);
        }
        catch (const winrt::hresult_error& ex) {
            // RPC_E_CHANGED_MODE (0x80010106) means apartment already initialized
            // This is OK - we can continue
            if (ex.code() != winrt::hresult(static_cast<int32_t>(0x80010106))) {
                // Different error - can't continue
                return false;
            }
            // Otherwise, continue - apartment is already initialized
        }
        
        // Create toast notifier with our AppUserModelID
        std::wstring wideAppId = toWide(appId);
        
        if (wideAppId.empty()) {
            return false;
        }
        
        auto notifier = winrt::Windows::UI::Notifications::ToastNotificationManager::CreateToastNotifier(wideAppId);
        
        // Store WinRT object directly in std::any (by value/move)
        // WinRT types are reference-counted, so copying is cheap
        toastNotifier_ = std::move(notifier);
        
        initialized_ = true;
        return true;
    }
    catch (const winrt::hresult_error&) {
        // Log or handle specific WinRT errors if needed
        return false;
    }
    catch (...) {
        return false;
    }
}

// ==============================================================================
// Show Methods
// ==============================================================================

ErrorInfo ToastNotifier::Impl::show(const ToastContent& content) {
    if (!initialized_ || !toastNotifier_.has_value()) {
        return Errors::NotInitialized;
    }
    
    if (!content.title) {
        return Errors::InvalidParameter;
    }
    
    try {
        // Generate toast XML
        std::wstring xml = generateToastXml(content, nullptr, 0);
        
        // Create XML document
        XmlDocument xmlDoc;
        xmlDoc.LoadXml(xml);
        
        // Create toast notification
        ToastNotification toast(xmlDoc);
        
        // Get notifier and show toast
        auto& notifier = getNotifier(toastNotifier_);
        notifier.Show(toast);
        
        return Errors::Success;
    }
    catch (...) {
        return Errors::ToastFailed;
    }
}

ErrorInfo ToastNotifier::Impl::showWithButtons(
    const ToastContent& content,
    const ToastButton* buttons,
    size_t buttonCount
) {
    if (!initialized_ || !toastNotifier_.has_value()) {
        return Errors::NotInitialized;
    }
    
    if (!content.title) {
        return Errors::InvalidParameter;
    }
    
    if (buttonCount > 5) {
        return Errors::InvalidParameter;  // Max 5 buttons
    }
    
    try {
        // Generate toast XML with buttons
        std::wstring xml = generateToastXml(content, buttons, buttonCount);
        
        // Create XML document
        XmlDocument xmlDoc;
        xmlDoc.LoadXml(xml);
        
        // Create toast notification
        ToastNotification toast(xmlDoc);
        
        // Register activation handler if callback is set
        if (activatedCallback_) {
            toast.Activated([this](ToastNotification const&,
                                  winrt::Windows::Foundation::IInspectable const& args) {
                try {
                    auto activatedArgs = args.as<ToastActivatedEventArgs>();
                    std::string arguments = toNarrow(activatedArgs.Arguments().c_str());
                    
                    // Get user input (if any)
                    std::string userInput;
                    auto inputs = activatedArgs.UserInput();
                    if (inputs.Size() > 0) {
                        auto firstInput = inputs.First().Current();
                        userInput = toNarrow(firstInput.Value().as<winrt::hstring>().c_str());
                    }
                    
                    // Call user callback
                    if (activatedCallback_) {
                        activatedCallback_(arguments.c_str(), 
                                         userInput.empty() ? nullptr : userInput.c_str());
                    }
                }
                catch (...) {
                    // Ignore callback errors
                }
            });
        }
        
        // Register dismissed handler if callback is set
        if (dismissedCallback_) {
            toast.Dismissed([this](ToastNotification const&,
                                  ToastDismissedEventArgs const&) {
                if (dismissedCallback_) {
                    dismissedCallback_();
                }
            });
        }
        
        // Get notifier and show toast
        auto& notifier = getNotifier(toastNotifier_);
        notifier.Show(toast);
        
        return Errors::Success;
    }
    catch (...) {
        return Errors::ToastFailed;
    }
}

ErrorInfo ToastNotifier::Impl::showWithInput(
    const ToastContent& content,
    const ToastInput& input,
    const ToastButton* buttons,
    size_t buttonCount
) {
    if (!initialized_ || !toastNotifier_.has_value()) {
        return Errors::NotInitialized;
    }
    
    if (!content.title || !input.id) {
        return Errors::InvalidParameter;
    }
    
    if (buttonCount > 5) {
        return Errors::InvalidParameter;  // Max 5 buttons
    }
    
    try {
        // Generate toast XML with input field
        std::wstring xml = generateToastXmlWithInput(content, input, buttons, buttonCount);
        
        // Create XML document
        XmlDocument xmlDoc;
        xmlDoc.LoadXml(xml);
        
        // Create toast notification
        ToastNotification toast(xmlDoc);
        
        // Register activation handler if callback is set
        if (activatedCallback_) {
            toast.Activated([this](ToastNotification const&,
                                  winrt::Windows::Foundation::IInspectable const& args) {
                try {
                    auto activatedArgs = args.as<ToastActivatedEventArgs>();
                    std::string arguments = toNarrow(activatedArgs.Arguments().c_str());
                    
                    // Get user input
                    std::string userInput;
                    auto inputs = activatedArgs.UserInput();
                    if (inputs.Size() > 0) {
                        auto firstInput = inputs.First().Current();
                        userInput = toNarrow(firstInput.Value().as<winrt::hstring>().c_str());
                    }
                    
                    if (activatedCallback_) {
                        activatedCallback_(arguments.c_str(), 
                                         userInput.empty() ? nullptr : userInput.c_str());
                    }
                }
                catch (...) {
                    // Ignore callback errors
                }
            });
        }
        
        // Register dismissed handler if callback is set
        if (dismissedCallback_) {
            toast.Dismissed([this](ToastNotification const&,
                                  ToastDismissedEventArgs const&) {
                if (dismissedCallback_) {
                    dismissedCallback_();
                }
            });
        }
        
        // Get notifier and show toast
        auto& notifier = getNotifier(toastNotifier_);
        notifier.Show(toast);
        
        return Errors::Success;
    }
    catch (...) {
        return Errors::ToastFailed;
    }
}

ErrorInfo ToastNotifier::Impl::showWithProgress(
    const ToastContent& content,
    const ToastProgress& progress,
    const char* tag
) {
    if (!initialized_ || !toastNotifier_.has_value()) {
        return Errors::NotInitialized;
    }
    
    if (!content.title || !tag) {
        return Errors::InvalidParameter;
    }
    
    try {
        // Generate toast XML with progress bar
        std::wstring xml = generateToastXmlWithProgress(content, progress);
        
        // Create XML document
        XmlDocument xmlDoc;
        xmlDoc.LoadXml(xml);
        
        // Create toast notification with tag
        ToastNotification toast(xmlDoc);
        toast.Tag(toWide(tag));
        
        // Set data for progress
        auto data = NotificationData();
        data.Values().Insert(L"progressValue", winrt::hstring(std::to_wstring(progress.value)));
        if (progress.valueString) {
            data.Values().Insert(L"progressValueString", winrt::hstring(toWide(progress.valueString)));
        }
        if (progress.status) {
            data.Values().Insert(L"progressStatus", winrt::hstring(toWide(progress.status)));
        }
        
        toast.Data(data);
        
        // Register activation handler if callback is set
        if (activatedCallback_) {
            toast.Activated([this](ToastNotification const&,
                                  winrt::Windows::Foundation::IInspectable const& args) {
                try {
                    auto activatedArgs = args.as<ToastActivatedEventArgs>();
                    std::string arguments = toNarrow(activatedArgs.Arguments().c_str());
                    
                    if (activatedCallback_) {
                        activatedCallback_(arguments.c_str(), nullptr);
                    }
                }
                catch (...) {
                    // Ignore callback errors
                }
            });
        }
        
        // Register dismissed handler if callback is set
        if (dismissedCallback_) {
            toast.Dismissed([this](ToastNotification const&,
                                  ToastDismissedEventArgs const&) {
                if (dismissedCallback_) {
                    dismissedCallback_();
                }
            });
        }
        
        // Get notifier and show toast
        auto& notifier = getNotifier(toastNotifier_);
        notifier.Show(toast);
        
        return Errors::Success;
    }
    catch (...) {
        return Errors::ToastFailed;
    }
}

// ==============================================================================
// Update Methods (Phase 4 TODO)
// ==============================================================================

ErrorInfo ToastNotifier::Impl::updateProgress(
    const char* tag,
    const ToastProgress& progress
) {
    if (!initialized_ || !toastNotifier_.has_value()) {
        return Errors::NotInitialized;
    }
    
    if (!tag) {
        return Errors::InvalidParameter;
    }
    
    try {
        // Create update data
        auto data = NotificationData();
        data.SequenceNumber(1);  // Increment for updates
        
        // Update progress value
        data.Values().Insert(L"progressValue", winrt::hstring(std::to_wstring(progress.value)));
        
        if (progress.valueString) {
            data.Values().Insert(L"progressValueString", winrt::hstring(toWide(progress.valueString)));
        }
        
        if (progress.status) {
            data.Values().Insert(L"progressStatus", winrt::hstring(toWide(progress.status)));
        }
        
        // Update the toast
        auto& notifier = getNotifier(toastNotifier_);
        auto result = notifier.Update(data, toWide(tag));
        
        if (result == NotificationUpdateResult::Succeeded) {
            return Errors::Success;
        } else {
            return Errors::ToastFailed;
        }
    }
    catch (...) {
        return Errors::ToastFailed;
    }
}

ErrorInfo ToastNotifier::Impl::clearAll() {
    if (!initialized_ || !toastNotifier_.has_value()) {
        return Errors::NotInitialized;
    }
    
    try {
        auto history = winrt::Windows::UI::Notifications::ToastNotificationManager::History();
        std::wstring wideAppId = toWide(appId_.c_str());
        history.Clear(wideAppId);
        
        return Errors::Success;
    }
    catch (...) {
        return Errors::ToastFailed;
    }
}

ErrorInfo ToastNotifier::Impl::clearByTag(const char* tag) {
    if (!initialized_ || !toastNotifier_.has_value()) {
        return Errors::NotInitialized;
    }
    
    if (!tag) {
        return Errors::InvalidParameter;
    }
    
    try {
        auto history = winrt::Windows::UI::Notifications::ToastNotificationManager::History();
        std::wstring wideTag = toWide(tag);
        std::wstring wideAppId = toWide(appId_.c_str());
        
        history.Remove(wideTag, {}, wideAppId);
        
        return Errors::Success;
    }
    catch (...) {
        return Errors::ToastFailed;
    }
}

// ==============================================================================
// Callbacks
// ==============================================================================

void ToastNotifier::Impl::onActivated(
    std::function<void(const char*, const char*)> callback
) {
    activatedCallback_ = std::move(callback);
}

void ToastNotifier::Impl::onDismissed(std::function<void()> callback) {
    dismissedCallback_ = std::move(callback);
}

// ==============================================================================
// Helper Methods
// ==============================================================================

std::wstring ToastNotifier::Impl::toWide(const char* str) {
    if (!str) return L"";
    
    int size = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
    if (size <= 0) return L"";
    
    std::wstring result(size - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, str, -1, result.data(), size);
    
    return result;
}

std::string ToastNotifier::Impl::toNarrow(const wchar_t* wstr) {
    if (!wstr) return "";
    
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, nullptr, 0, nullptr, nullptr);
    if (size <= 0) return "";
    
    std::string result(size - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, result.data(), size, nullptr, nullptr);
    
    return result;
}

std::wstring ToastNotifier::Impl::generateToastXml(
    const ToastContent& content,
    const ToastButton* buttons,
    size_t buttonCount
) {
    std::wostringstream xml;
    
    // Start toast element
    xml << L"<toast";
    
    // Add duration attribute
    if (content.duration == ToastDuration::Long) {
        xml << L" duration=\"long\"";
    }
    
    // Add scenario attribute
    switch (content.scenario) {
        case ToastScenario::Reminder:
            xml << L" scenario=\"reminder\"";
            break;
        case ToastScenario::Alarm:
            xml << L" scenario=\"alarm\"";
            break;
        case ToastScenario::IncomingCall:
            xml << L" scenario=\"incomingCall\"";
            break;
        default:
            break;
    }
    
    xml << L">";
    
    // Visual element
    xml << L"<visual>";
    xml << L"<binding template=\"ToastGeneric\">";
    
    // Title (required)
    if (content.title) {
        xml << L"<text>" << toWide(content.title) << L"</text>";
    }
    
    // Body (optional)
    if (content.body) {
        xml << L"<text>" << toWide(content.body) << L"</text>";
    }
    
    // Attribution (optional)
    if (content.attribution) {
        xml << L"<text placement=\"attribution\">" << toWide(content.attribution) << L"</text>";
    }
    
    // Image (optional)
    if (content.imagePath) {
        xml << L"<image";
        
        // Image placement
        switch (content.imagePlacement) {
            case ToastImagePlacement::AppLogo:
                xml << L" placement=\"appLogoOverride\"";
                break;
            case ToastImagePlacement::Hero:
                xml << L" placement=\"hero\"";
                break;
            default:
                break;
        }
        
        xml << L" src=\"" << toWide(content.imagePath) << L"\"";
        xml << L"/>";
    }
    
    xml << L"</binding>";
    xml << L"</visual>";
    
    // Audio element (optional)
    if (content.audio != ToastAudio::Default) {
        xml << L"<audio";
        
        if (content.audio == ToastAudio::Silent) {
            xml << L" silent=\"true\"";
        } else if (content.audio == ToastAudio::Loop) {
            xml << L" loop=\"true\"";
        }
        
        xml << L"/>";
    }
    
    // Actions element (buttons)
    if (buttons && buttonCount > 0) {
        xml << L"<actions>";
        
        for (size_t i = 0; i < buttonCount && i < 5; ++i) {
            if (buttons[i].label && buttons[i].arguments) {
                xml << L"<action";
                xml << L" content=\"" << toWide(buttons[i].label) << L"\"";
                xml << L" arguments=\"" << toWide(buttons[i].arguments) << L"\"";
                xml << L" activationType=\"foreground\"";
                
                if (buttons[i].iconPath) {
                    xml << L" imageUri=\"" << toWide(buttons[i].iconPath) << L"\"";
                }
                
                xml << L"/>";
            }
        }
        
        xml << L"</actions>";
    }
    
    xml << L"</toast>";
    
    return xml.str();
}

std::wstring ToastNotifier::Impl::generateToastXmlWithInput(
    const ToastContent& content,
    const ToastInput& input,
    const ToastButton* buttons,
    size_t buttonCount
) {
    std::wostringstream xml;
    
    // Start toast element
    xml << L"<toast";
    
    if (content.duration == ToastDuration::Long) {
        xml << L" duration=\"long\"";
    }
    
    switch (content.scenario) {
        case ToastScenario::Reminder:
            xml << L" scenario=\"reminder\"";
            break;
        case ToastScenario::Alarm:
            xml << L" scenario=\"alarm\"";
            break;
        case ToastScenario::IncomingCall:
            xml << L" scenario=\"incomingCall\"";
            break;
        default:
            break;
    }
    
    xml << L">";
    
    // Visual element
    xml << L"<visual>";
    xml << L"<binding template=\"ToastGeneric\">";
    
    if (content.title) {
        xml << L"<text>" << toWide(content.title) << L"</text>";
    }
    
    if (content.body) {
        xml << L"<text>" << toWide(content.body) << L"</text>";
    }
    
    if (content.attribution) {
        xml << L"<text placement=\"attribution\">" << toWide(content.attribution) << L"</text>";
    }
    
    if (content.imagePath) {
        xml << L"<image";
        
        switch (content.imagePlacement) {
            case ToastImagePlacement::AppLogo:
                xml << L" placement=\"appLogoOverride\"";
                break;
            case ToastImagePlacement::Hero:
                xml << L" placement=\"hero\"";
                break;
            default:
                break;
        }
        
        xml << L" src=\"" << toWide(content.imagePath) << L"\"";
        xml << L"/>";
    }
    
    xml << L"</binding>";
    xml << L"</visual>";
    
    // Audio element
    if (content.audio != ToastAudio::Default) {
        xml << L"<audio";
        
        if (content.audio == ToastAudio::Silent) {
            xml << L" silent=\"true\"";
        } else if (content.audio == ToastAudio::Loop) {
            xml << L" loop=\"true\"";
        }
        
        xml << L"/>";
    }
    
    // Actions element (input + buttons)
    xml << L"<actions>";
    
    // Add input field
    xml << L"<input id=\"" << toWide(input.id) << L"\" type=\"text\"";
    
    if (input.placeholder) {
        xml << L" placeHolderContent=\"" << toWide(input.placeholder) << L"\"";
    }
    
    if (input.defaultValue) {
        xml << L" defaultInput=\"" << toWide(input.defaultValue) << L"\"";
    }
    
    xml << L"/>";
    
    // Add buttons
    for (size_t i = 0; i < buttonCount && i < 5; ++i) {
        if (buttons[i].label && buttons[i].arguments) {
            xml << L"<action";
            xml << L" content=\"" << toWide(buttons[i].label) << L"\"";
            xml << L" arguments=\"" << toWide(buttons[i].arguments) << L"\"";
            xml << L" activationType=\"foreground\"";
            
            if (buttons[i].iconPath) {
                xml << L" imageUri=\"" << toWide(buttons[i].iconPath) << L"\"";
            }
            
            xml << L"/>";
        }
    }
    
    xml << L"</actions>";
    xml << L"</toast>";
    
    return xml.str();
}

std::wstring ToastNotifier::Impl::generateToastXmlWithProgress(
    const ToastContent& content,
    const ToastProgress& progress
) {
    std::wostringstream xml;
    
    // Start toast element
    xml << L"<toast";
    
    if (content.duration == ToastDuration::Long) {
        xml << L" duration=\"long\"";
    }
    
    switch (content.scenario) {
        case ToastScenario::Reminder:
            xml << L" scenario=\"reminder\"";
            break;
        case ToastScenario::Alarm:
            xml << L" scenario=\"alarm\"";
            break;
        case ToastScenario::IncomingCall:
            xml << L" scenario=\"incomingCall\"";
            break;
        default:
            break;
    }
    
    xml << L">";
    
    // Visual element
    xml << L"<visual>";
    xml << L"<binding template=\"ToastGeneric\">";
    
    if (content.title) {
        xml << L"<text>" << toWide(content.title) << L"</text>";
    }
    
    if (content.body) {
        xml << L"<text>" << toWide(content.body) << L"</text>";
    }
    
    if (content.attribution) {
        xml << L"<text placement=\"attribution\">" << toWide(content.attribution) << L"</text>";
    }
    
    // Progress bar
    xml << L"<progress";
    
    if (progress.title) {
        xml << L" title=\"" << toWide(progress.title) << L"\"";
    }
    
    xml << L" value=\"{progressValue}\"";
    xml << L" valueStringOverride=\"{progressValueString}\"";
    xml << L" status=\"{progressStatus}\"";
    xml << L"/>";
    
    if (content.imagePath) {
        xml << L"<image";
        
        switch (content.imagePlacement) {
            case ToastImagePlacement::AppLogo:
                xml << L" placement=\"appLogoOverride\"";
                break;
            case ToastImagePlacement::Hero:
                xml << L" placement=\"hero\"";
                break;
            default:
                break;
        }
        
        xml << L" src=\"" << toWide(content.imagePath) << L"\"";
        xml << L"/>";
    }
    
    xml << L"</binding>";
    xml << L"</visual>";
    
    // Audio element
    if (content.audio != ToastAudio::Default) {
        xml << L"<audio";
        
        if (content.audio == ToastAudio::Silent) {
            xml << L" silent=\"true\"";
        } else if (content.audio == ToastAudio::Loop) {
            xml << L" loop=\"true\"";
        }
        
        xml << L"/>";
    }
    
    xml << L"</toast>";
    
    return xml.str();
}

} // namespace windows_platform
