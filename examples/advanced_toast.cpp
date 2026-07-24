// ==============================================================================
// advanced_toast.cpp - Advanced Toast with Buttons & Image
// ==============================================================================

#include <toast_notifications/toast_notification.h>
#include <cstdio>
#include <thread>
#include <chrono>

int main() {
    printf("========================================\n");
    printf("Advanced Toast Notification Example\n");
    printf("========================================\n\n");
    
    // Create toast notification manager
    printf("[1/4] Creating toast notification manager...\n");
    auto toast = windows_platform::ToastNotifier::create("AdvancedToast.Example");
    
    if (!toast) {
        fprintf(stderr, "[ERROR] Failed to create ToastNotifier\n");
        return 1;
    }
    
    printf("[OK] ToastNotifier created\n\n");
    
    // Register activation callback
    printf("[2/4] Registering activation callback...\n");
    toast->onActivated([](const char* args, const char* input) {
        printf("\n[CALLBACK] Toast activated!\n");
        if (args && strlen(args) > 0) {
            printf("           Arguments: %s\n", args);
        }
        if (input && strlen(input) > 0) {
            printf("           Input: %s\n", input);
        }
    });
    
    toast->onDismissed([]() {
        printf("\n[CALLBACK] Toast dismissed\n");
    });
    
    printf("[OK] Callbacks registered\n\n");
    
    // Prepare toast with buttons
    printf("[3/4] Preparing toast with buttons...\n");
    windows_platform::ToastContent content;
    content.title = "Now Playing";
    content.body = "Hotel California\nEagles";
    content.duration = windows_platform::ToastDuration::Short;
    // content.imagePath = "file:///C:/Music/cover.jpg";  // TODO: Add your image
    
    windows_platform::ToastButton buttons[] = {
        { "▶️ Play", "action=play" },
        { "⏭️ Next", "action=next" },
        { "❤️ Like", "action=like" }
    };
    
    printf("[OK] Toast prepared\n\n");
    
    // Show toast with buttons
    printf("[4/4] Showing toast with buttons...\n");
    auto error = toast->showWithButtons(content, buttons, 3);
    
    if (!error.isSuccess()) {
        fprintf(stderr, "[ERROR] Failed to show toast: %s\n", error.message);
        return 1;
    }
    
    printf("[OK] Toast shown with action buttons!\n\n");
    
    printf("========================================\n");
    printf("Check your Action Center (Win+A)\n");
    printf("Click the buttons to see callbacks!\n");
    printf("========================================\n\n");
    
    // Wait for potential callbacks (5 seconds)
    printf("Waiting for interactions (5 seconds)...\n");
    std::this_thread::sleep_for(std::chrono::seconds(5));
    
    return 0;
}
