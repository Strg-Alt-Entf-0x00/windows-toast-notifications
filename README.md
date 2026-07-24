# Windows Toast Notifications

[![Version](https://img.shields.io/badge/version-0.9.0-blue.svg)](https://github.com/KernelMeltdown/windows-toast-notifications)
[![License](https://img.shields.io/badge/license-Proprietary-red.svg)](LICENSE)
[![C++](https://img.shields.io/badge/C++-20-orange.svg)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/platform-Windows-lightgrey.svg)](https://www.microsoft.com/windows)
[![Free for Non-Commercial](https://img.shields.io/badge/free-non--commercial-brightgreen.svg)](LICENSE)

C++20 library for Windows Toast Notifications using WinRT APIs.

## Features

- Text, images, progress bars, and buttons
- Pre-built layouts for common notification types
- Clickable buttons and input fields
- Persistent notifications with history
- Real-time progress bar updates
- Scheduled notifications
- C++20 with WinRT coroutines

## Requirements

- **CMake**: 3.20 or higher
- **Compiler**: C++20 with WinRT support
  - MSVC 2019 16.11+ (Visual Studio 2019)
  - MSVC 2022 (recommended)
- **Platform**: Windows 10 1809+ or Windows 11
- **Windows SDK**: 10.0.17763.0 or higher

## Quick Start

### Building

```bash
# Using the provided build script
build.bat

# Or manually
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

### Basic Toast Notification

```cpp
#include <toast_notifications/toast_notification.h>

int main() {
    // Create toast manager
    toast::ToastNotification toaster(L"MyApp.AppID");
    
    // Show simple toast
    toaster.show_simple(L"Hello World", L"This is a toast notification!");
    
    return 0;
}
```

### Toast with Buttons

```cpp
#include <toast_notifications/toast_notification.h>

int main() {
    toast::ToastNotification toaster(L"MyApp.AppID");
    
    // Create toast with buttons
    toast::ToastBuilder builder;
    builder.set_title(L"New Message")
           .set_message(L"You have a new message from John")
           .add_button(L"Reply", L"reply_action")
           .add_button(L"Dismiss", L"dismiss_action");
    
    // Show and handle response
    toaster.show(builder, [](const std::wstring& action) {
        if (action == L"reply_action") {
            // Handle reply
        }
    });
    
    return 0;
}
```

### Progress Toast

```cpp
#include <toast_notifications/toast_notification.h>

int main() {
    toast::ToastNotification toaster(L"MyApp.AppID");
    
    // Show progress toast
    auto toast_id = toaster.show_progress(L"Downloading", 
                                          L"Downloading file...", 
                                          0.0);
    
    // Update progress
    for (int i = 0; i <= 100; i += 10) {
        toaster.update_progress(toast_id, i / 100.0, 
                                L"Downloading: " + std::to_wstring(i) + L"%");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return 0;
}
```

### Toast with Image

```cpp
#include <toast_notifications/toast_notification.h>

toast::ToastBuilder builder;
builder.set_title(L"Photo Uploaded")
       .set_message(L"Your photo has been uploaded successfully")
       .set_image(L"C:\\Images\\photo.jpg")
       .set_app_logo(L"C:\\Icons\\app_logo.png");

toaster.show(builder);
```

## API Reference

### ToastNotification Class

#### Constructor
```cpp
ToastNotification(const std::wstring& app_id);
```

#### Simple Notifications
```cpp
void show_simple(const std::wstring& title, 
                 const std::wstring& message);
```

#### Advanced Notifications
```cpp
toast_id_t show(const ToastBuilder& builder, 
                std::function<void(const std::wstring&)> callback = nullptr);
```

#### Progress Toasts
```cpp
toast_id_t show_progress(const std::wstring& title,
                         const std::wstring& message,
                         double progress);

void update_progress(toast_id_t id, double progress, 
                     const std::wstring& status);
```

#### Management
```cpp
void dismiss(toast_id_t id);
void clear_history();
```

### ToastBuilder Class

#### Content
```cpp
ToastBuilder& set_title(const std::wstring& title);
ToastBuilder& set_message(const std::wstring& message);
ToastBuilder& set_image(const std::wstring& image_path);
ToastBuilder& set_app_logo(const std::wstring& logo_path);
```

#### Interactions
```cpp
ToastBuilder& add_button(const std::wstring& content, 
                         const std::wstring& action_id);
ToastBuilder& add_input(const std::wstring& id, 
                        const std::wstring& placeholder);
```

#### Behavior
```cpp
ToastBuilder& set_duration(ToastDuration duration);
ToastBuilder& set_scenario(ToastScenario scenario);
ToastBuilder& set_audio(ToastAudio audio);
```

Toast durations:
- `ToastDuration::Short` - 7 seconds
- `ToastDuration::Long` - 25 seconds

Toast scenarios:
- `ToastScenario::Default`
- `ToastScenario::Reminder` - Stays on screen until dismissed
- `ToastScenario::Alarm` - Audio loops, stays until dismissed
- `ToastScenario::IncomingCall` - Special incoming call UI

Audio options:
- `ToastAudio::Default`
- `ToastAudio::Silent`
- `ToastAudio::Mail`
- `ToastAudio::Reminder`
- `ToastAudio::SMS`

## CMake Integration

```cmake
# In your CMakeLists.txt
add_subdirectory(third-party-cpp/windows-toast-notifications-0.9.0)

add_executable(your_app main.cpp)
target_link_libraries(your_app PRIVATE windows-toast-notifications)
```

## Configuration Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_EXAMPLES` | `OFF` | Build example programs |
| `BUILD_TESTING` | `OFF` | Build unit tests |

## Examples

The library includes several example programs:

- `basic_toast.cpp` - Simple toast notifications
- `advanced_toast.cpp` - Rich notifications with buttons
- `persistent_test.cpp` - Long-lived notifications
- `visual_test.cpp` - Image and logo testing
- `test_with_shortcut.cpp` - App registration demo

Build examples:
```bash
cd build
cmake -DBUILD_EXAMPLES=ON ..
cmake --build . --config Release
```

## Application ID (AppID)

Windows requires an Application User Model ID (AppID) for toast notifications. 

### For Packaged Apps (Microsoft Store)
The AppID is defined in your app manifest.

### For Unpackaged Apps (Desktop)
You need to:

1. Register a shortcut in the Start Menu
2. Use that shortcut's AppID

The library includes helper functions:

```cpp
#include <toast_notifications/toast_notification.h>

// Create and register shortcut
toast::create_shortcut(
    L"MyApp",                           // App name
    L"C:\\MyApp\\myapp.exe",           // Executable path
    L"MyApp.AppID"                      // AppID
);

// Now you can use toasts
toast::ToastNotification toaster(L"MyApp.AppID");
```

## Advanced Features

### Custom XML Templates

```cpp
std::wstring custom_xml = LR"(
<toast>
    <visual>
        <binding template="ToastGeneric">
            <text>Custom Toast</text>
            <text>Using raw XML</text>
        </binding>
    </visual>
    <actions>
        <action content="Open" arguments="open"/>
    </actions>
</toast>
)";

toaster.show_from_xml(custom_xml);
```

### Notification Groups

```cpp
toast::ToastBuilder builder;
builder.set_group(L"messages")
       .set_tag(L"msg_001");  // Unique ID within group
```

### Attribution Text

```cpp
builder.set_attribution(L"via Email");
```

## Project Structure

```
windows-toast-notifications-0.9.0/
├── include/
│   └── toast_notifications/
│       ├── toast_notification.h
│       └── types.h
├── src/
│   ├── toast_notification.cpp
│   ├── toast_notification_impl.cpp
│   ├── toast_notification_impl.h
│   ├── shortcut_manager.cpp
│   ├── appid_manager.cpp
│   └── appid_manager.h
├── examples/
│   ├── basic_toast.cpp
│   ├── advanced_toast.cpp
│   ├── persistent_test.cpp
│   ├── visual_test.cpp
│   └── test_with_shortcut.cpp
├── tests/
├── build.bat
├── CMakeLists.txt
├── LICENSE
└── README.md
```

## Best Practices

### Image Paths
- Use absolute file paths
- Recommended size: 364x180 pixels for hero images
- Supported formats: JPG, PNG
- Keep under 200KB for performance

### Text Length
- Title: Max 2 lines (~40 characters)
- Message: Max 4 lines (~80 characters)
- Button text: Keep short (~15 characters)

### Action Handling
Always handle toast actions promptly:
```cpp
toaster.show(builder, [](const std::wstring& action) {
    // Handle action immediately
    // Don't do heavy work here
});
```

## Troubleshooting

### Toasts Not Appearing
- Check that notifications are enabled in Windows Settings
- Verify AppID is correctly registered
- For desktop apps, ensure shortcut exists in Start Menu
- Check Focus Assist settings

### Images Not Loading
- Use absolute file paths
- Verify file exists and has read permissions
- Check image format is supported
- Ensure file size is reasonable

### Actions Not Working
- Verify callback is registered
- Check action IDs are unique
- Ensure app responds quickly to activation

## Windows Notification Settings

Users can control your app's notifications in:
- Settings → System → Notifications
- Focus Assist settings
- Per-app notification settings

Your app should respect these settings - Windows handles this automatically.

## License

See [LICENSE](LICENSE) file for details.

**Free for non-commercial use. Commercial use requires a license.**