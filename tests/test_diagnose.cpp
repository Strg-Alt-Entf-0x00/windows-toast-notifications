// ==============================================================================
// test_diagnose.cpp - Diagnostic Test for Library Issues
// ==============================================================================

#include <toast_notifications/toast_notification.h>
#include <toast_notifications/types.h>
#include <toast_notifications/error.h>
#include <cstdio>

using namespace windows_platform;

int main() {
    printf("\n");
    printf("========================================\n");
    printf("windows-toast-notifications-0.9.0\n");
    printf("Diagnostic Test\n");
    printf("========================================\n");
    printf("\n");
    
    // Step 1: Test factory method
    printf("[1/2] Testing ToastNotifier::create()...\n");
    
    auto toast = ToastNotifier::create("DiagnoseTest.Factory");
    
    if (toast) {
        printf("      [OK] ToastNotifier::create() succeeded\n");
    } else {
        printf("      [ERROR] ToastNotifier::create() returned nullptr\n");
        printf("             Possible causes:\n");
        printf("             - Windows version check failed (need Windows 10+)\n");
        printf("             - AppUserModelID registration failed\n");
        printf("             - WinRT initialization failed\n");
        printf("             - Internal initialization error\n");
        printf("\n");
        return 1;
    }
    
    printf("\n");
    
    // Step 2: Test show() method
    printf("[2/2] Testing toast.show()...\n");
    
    ToastContent content;
    content.title = "Diagnostic Test";
    content.body = "If you see this, the library works!";
    
    auto error = toast->show(content);
    
    if (error.isSuccess()) {
        printf("      [OK] toast.show() succeeded\n");
        printf("\n");
        printf("========================================\n");
        printf("[SUCCESS] ALL DIAGNOSTICS PASSED!\n");
        printf("========================================\n");
        printf("\n");
        printf("Check your notification center to see the toast.\n");
        printf("The toast should say: 'Diagnostic Test'\n");
        printf("\n");
        return 0;
    } else {
        printf("      [ERROR] toast.show() failed\n");
        printf("             Error code: %d\n", error.code);
        printf("             Error message: %s\n", error.message);
        printf("\n");
        return 1;
    }
}
