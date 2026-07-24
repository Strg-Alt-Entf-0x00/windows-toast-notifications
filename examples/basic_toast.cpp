// ==============================================================================
// basic_toast.cpp - Simple Toast Notification Example
// ==============================================================================

#include <toast_notifications/toast_notification.h>
#include <cstdio>
#include <thread>
#include <chrono>

int main() {
    printf("========================================\n");
    printf("Basic Toast Notification Example\n");
    printf("========================================\n\n");
    
    // Create toast notification manager
    printf("[1/3] Creating toast notification manager...\n");
    auto toast = windows_platform::ToastNotifier::create("BasicToast.Example");
    
    if (!toast) {
        fprintf(stderr, "[ERROR] Failed to create ToastNotifier\n");
        fprintf(stderr, "        Make sure you're running Windows 10+\n");
        return 1;
    }
    
    printf("[OK] ToastNotifier created successfully\n\n");
    
    // Prepare toast content
    printf("[2/3] Preparing toast content...\n");
    windows_platform::ToastContent content;
    content.title = "Hello from Toast!";
    content.body = "This is a basic toast notification example.\nBuilt with windows-toast-notifications-0.9.0";
    content.duration = windows_platform::ToastDuration::Short;
    
    printf("[OK] Toast content prepared\n\n");
    
    // Show toast
    printf("[3/3] Showing toast notification...\n");
    auto error = toast->show(content);
    
    if (!error.isSuccess()) {
        fprintf(stderr, "[ERROR] Failed to show toast: %s\n", error.message);
        return 1;
    }
    
    printf("[OK] Toast notification shown!\n\n");
    
    printf("========================================\n");
    printf("Check your Action Center (Win+A) to see\n");
    printf("the notification!\n");
    printf("========================================\n\n");
    
    // Wait a moment to let the toast appear
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    return 0;
}
