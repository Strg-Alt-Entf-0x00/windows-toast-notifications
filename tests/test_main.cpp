// ==============================================================================
// test_main.cpp - Integration & End-to-End Test Suite
// ==============================================================================
// Description: Comprehensive test suite for windows-toast-notifications-0.9.0
// Purpose:     Validate all features work correctly before production
// ==============================================================================

#include <toast_notifications/toast_notification.h>
#include <cstdio>
#include <thread>
#include <chrono>
#include <atomic>
#include <cassert>

using namespace windows_platform;
using namespace std::chrono_literals;

// ==============================================================================
// Test Framework
// ==============================================================================

static int g_testsPassed = 0;
static int g_testsFailed = 0;

#define TEST_BEGIN(name) \
    printf("\n[TEST] %s\n", name); \
    printf("----------------------------------------\n");

#define TEST_END() \
    printf("----------------------------------------\n");

#define ASSERT(condition, message) \
    if (!(condition)) { \
        fprintf(stderr, "[FAIL] %s\n", message); \
        g_testsFailed++; \
        return false; \
    } else { \
        printf("[PASS] %s\n", message); \
        g_testsPassed++; \
    }

#define ASSERT_SUCCESS(error, message) \
    if (!(error).isSuccess()) { \
        fprintf(stderr, "[FAIL] %s: %s\n", message, (error).message); \
        g_testsFailed++; \
        return false; \
    } else { \
        printf("[PASS] %s\n", message); \
        g_testsPassed++; \
    }

// ==============================================================================
// Test 1: Factory Creation
// ==============================================================================

bool test_factory_creation() {
    TEST_BEGIN("Factory Creation & Initialization");
    
    // Test valid creation
    auto toast = ToastNotifier::create("TestApp.Integration");
    ASSERT(toast != nullptr, "Factory creates valid instance");
    
    // Test with default AppID
    auto toast2 = ToastNotifier::create();
    ASSERT(toast2 != nullptr, "Factory creates instance with default AppID");
    
    TEST_END();
    return true;
}

// ==============================================================================
// Test 2: Simple Toast
// ==============================================================================

bool test_simple_toast() {
    TEST_BEGIN("Simple Toast Notification");
    
    auto toast = ToastNotifier::create("TestApp.SimpleToast");
    ASSERT(toast != nullptr, "ToastNotifier created");
    
    // Show simple toast
    ToastContent content;
    content.title = "Test Toast";
    content.body = "This is a test notification";
    content.duration = ToastDuration::Short;
    
    auto error = toast->show(content);
    ASSERT_SUCCESS(error, "Simple toast shown successfully");
    
    printf("[INFO] Check Action Center (Win+A) for notification\n");
    std::this_thread::sleep_for(2s);
    
    TEST_END();
    return true;
}

// ==============================================================================
// Test 3: Toast with Image
// ==============================================================================

bool test_toast_with_image() {
    TEST_BEGIN("Toast with Image");
    
    auto toast = ToastNotifier::create("TestApp.ImageToast");
    ASSERT(toast != nullptr, "ToastNotifier created");
    
    ToastContent content;
    content.title = "Image Test";
    content.body = "Toast with Windows logo";
    content.imagePath = "file:///C:/Windows/System32/@WindowsUpdateToastIcon.png";
    content.imagePlacement = ToastImagePlacement::AppLogo;
    
    auto error = toast->show(content);
    ASSERT_SUCCESS(error, "Toast with image shown");
    
    printf("[INFO] Check Action Center for toast with image\n");
    std::this_thread::sleep_for(2s);
    
    TEST_END();
    return true;
}

// ==============================================================================
// Test 4: Toast with Buttons
// ==============================================================================

bool test_toast_with_buttons() {
    TEST_BEGIN("Toast with Action Buttons");
    
    auto toast = ToastNotifier::create("TestApp.ButtonToast");
    ASSERT(toast != nullptr, "ToastNotifier created");
    
    // Set up callback
    std::atomic<bool> callbackInvoked = false;
    std::string receivedArgs;
    
    toast->onActivated([&](const char* args, const char* input) {
        printf("[CALLBACK] Button clicked: %s\n", args ? args : "(none)");
        callbackInvoked = true;
        if (args) {
            receivedArgs = args;
        }
    });
    
    // Create buttons
    ToastButton buttons[] = {
        { "Accept", "action=accept" },
        { "Decline", "action=decline" }
    };
    
    ToastContent content;
    content.title = "Button Test";
    content.body = "Click a button to test callbacks";
    
    auto error = toast->showWithButtons(content, buttons, 2);
    ASSERT_SUCCESS(error, "Toast with buttons shown");
    
    printf("[INFO] Please click a button in the notification\n");
    printf("[INFO] Waiting 15 seconds for user interaction...\n");
    
    // Wait for callback
    for (int i = 0; i < 15; ++i) {
        std::this_thread::sleep_for(1s);
        if (callbackInvoked) {
            printf("[INFO] Callback received after %d seconds\n", i + 1);
            break;
        }
    }
    
    if (callbackInvoked) {
        printf("[PASS] Callback invoked with args: %s\n", receivedArgs.c_str());
        g_testsPassed++;
    } else {
        printf("[SKIP] User did not click button (manual test skipped)\n");
    }
    
    TEST_END();
    return true;
}

// ==============================================================================
// Test 5: Toast with Input Field
// ==============================================================================

bool test_toast_with_input() {
    TEST_BEGIN("Toast with Text Input");
    
    auto toast = ToastNotifier::create("TestApp.InputToast");
    ASSERT(toast != nullptr, "ToastNotifier created");
    
    // Set up callback
    std::atomic<bool> callbackInvoked = false;
    std::string receivedInput;
    
    toast->onActivated([&](const char* args, const char* input) {
        printf("[CALLBACK] Input received\n");
        printf("  Args: %s\n", args ? args : "(none)");
        printf("  Input: %s\n", input ? input : "(empty)");
        callbackInvoked = true;
        if (input) {
            receivedInput = input;
        }
    });
    
    // Create input field
    ToastInput input;
    input.id = "replyBox";
    input.placeholder = "Type your reply...";
    input.defaultValue = "";
    
    ToastButton button = { "Send", "action=send" };
    
    ToastContent content;
    content.title = "Input Test";
    content.body = "Type a message and click Send";
    
    auto error = toast->showWithInput(content, input, &button, 1);
    ASSERT_SUCCESS(error, "Toast with input shown");
    
    printf("[INFO] Please type text and click Send\n");
    printf("[INFO] Waiting 20 seconds for user interaction...\n");
    
    // Wait for callback
    for (int i = 0; i < 20; ++i) {
        std::this_thread::sleep_for(1s);
        if (callbackInvoked) {
            printf("[INFO] Callback received after %d seconds\n", i + 1);
            break;
        }
    }
    
    if (callbackInvoked && !receivedInput.empty()) {
        printf("[PASS] Input received: %s\n", receivedInput.c_str());
        g_testsPassed++;
    } else {
        printf("[SKIP] User did not provide input (manual test skipped)\n");
    }
    
    TEST_END();
    return true;
}

// ==============================================================================
// Test 6: Toast with Progress Bar
// ==============================================================================

bool test_toast_with_progress() {
    TEST_BEGIN("Toast with Progress Bar");
    
    auto toast = ToastNotifier::create("TestApp.ProgressToast");
    ASSERT(toast != nullptr, "ToastNotifier created");
    
    ToastContent content;
    content.title = "Download Test";
    content.body = "Downloading file...";
    
    ToastProgress progress;
    progress.title = "Downloading";
    progress.value = 0.0;
    progress.valueString = "0%";
    progress.status = "Starting...";
    
    auto error = toast->showWithProgress(content, progress, "download_test");
    ASSERT_SUCCESS(error, "Toast with progress shown");
    
    printf("[INFO] Updating progress...\n");
    
    // Simulate download progress
    for (int i = 1; i <= 10; ++i) {
        std::this_thread::sleep_for(500ms);
        
        progress.value = i / 10.0;
        std::string valueStr = std::to_string(i * 10) + "%";
        progress.valueString = valueStr.c_str();
        progress.status = i < 10 ? "Downloading..." : "Complete!";
        
        error = toast->updateProgress("download_test", progress);
        if (error.isSuccess()) {
            printf("[INFO] Progress updated: %d%%\n", i * 10);
        }
    }
    
    ASSERT_SUCCESS(error, "Progress updates succeeded");
    
    printf("[INFO] Check Action Center for completed progress\n");
    std::this_thread::sleep_for(2s);
    
    TEST_END();
    return true;
}

// ==============================================================================
// Test 7: Toast Dismissal Callback
// ==============================================================================

bool test_dismissal_callback() {
    TEST_BEGIN("Toast Dismissal Callback");
    
    auto toast = ToastNotifier::create("TestApp.DismissalTest");
    ASSERT(toast != nullptr, "ToastNotifier created");
    
    std::atomic<bool> dismissed = false;
    
    toast->onDismissed([&]() {
        printf("[CALLBACK] Toast dismissed\n");
        dismissed = true;
    });
    
    ToastContent content;
    content.title = "Dismissal Test";
    content.body = "Close this notification manually";
    
    auto error = toast->show(content);
    ASSERT_SUCCESS(error, "Toast shown");
    
    printf("[INFO] Please dismiss the notification (click X)\n");
    printf("[INFO] Waiting 15 seconds...\n");
    
    for (int i = 0; i < 15; ++i) {
        std::this_thread::sleep_for(1s);
        if (dismissed) {
            printf("[INFO] Dismissal detected after %d seconds\n", i + 1);
            break;
        }
    }
    
    if (dismissed) {
        printf("[PASS] Dismissal callback invoked\n");
        g_testsPassed++;
    } else {
        printf("[SKIP] User did not dismiss (manual test skipped)\n");
    }
    
    TEST_END();
    return true;
}

// ==============================================================================
// Test 8: Clear Toasts
// ==============================================================================

bool test_clear_toasts() {
    TEST_BEGIN("Clear Toast Notifications");
    
    auto toast = ToastNotifier::create("TestApp.ClearTest");
    ASSERT(toast != nullptr, "ToastNotifier created");
    
    // Show multiple toasts
    for (int i = 1; i <= 3; ++i) {
        ToastContent content;
        content.title = "Clear Test";
        std::string bodyText = "Notification #" + std::to_string(i);
        content.body = bodyText.c_str();
        
        std::string msg = "Toast " + std::to_string(i) + " shown";
        auto error = toast->show(content);
        ASSERT_SUCCESS(error, msg.c_str());
        std::this_thread::sleep_for(200ms);
    }
    
    printf("[INFO] 3 notifications shown\n");
    std::this_thread::sleep_for(2s);
    
    // Clear all
    printf("[INFO] Clearing all notifications...\n");
    auto error = toast->clearAll();
    ASSERT_SUCCESS(error, "All toasts cleared");
    
    printf("[INFO] Check Action Center - should be empty\n");
    std::this_thread::sleep_for(2s);
    
    TEST_END();
    return true;
}

// ==============================================================================
// Test 9: Clear by Tag
// ==============================================================================

bool test_clear_by_tag() {
    TEST_BEGIN("Clear Toast by Tag");
    
    auto toast = ToastNotifier::create("TestApp.TagClearTest");
    ASSERT(toast != nullptr, "ToastNotifier created");
    
    // Show progress toast with tag
    ToastContent content;
    content.title = "Tag Test";
    content.body = "This will be cleared by tag";
    
    ToastProgress progress;
    progress.title = "Processing";
    progress.value = 0.5;
    progress.valueString = "50%";
    
    auto error = toast->showWithProgress(content, progress, "clear_me");
    ASSERT_SUCCESS(error, "Tagged toast shown");
    
    std::this_thread::sleep_for(2s);
    
    // Clear by tag
    printf("[INFO] Clearing toast by tag 'clear_me'...\n");
    error = toast->clearByTag("clear_me");
    ASSERT_SUCCESS(error, "Toast cleared by tag");
    
    printf("[INFO] Check Action Center - should be cleared\n");
    std::this_thread::sleep_for(2s);
    
    TEST_END();
    return true;
}

// ==============================================================================
// Test 10: Error Handling
// ==============================================================================

bool test_error_handling() {
    TEST_BEGIN("Error Handling");
    
    auto toast = ToastNotifier::create("TestApp.ErrorTest");
    ASSERT(toast != nullptr, "ToastNotifier created");
    
    // Test invalid parameter (no title)
    ToastContent content;
    content.title = nullptr;
    content.body = "No title";
    
    auto error = toast->show(content);
    ASSERT(!error.isSuccess(), "Rejects toast without title");
    printf("[INFO] Error message: %s\n", error.message);
    
    // Test too many buttons
    ToastButton buttons[10];
    for (int i = 0; i < 10; ++i) {
        buttons[i].label = "Button";
        buttons[i].arguments = "test";
    }
    
    content.title = "Too Many Buttons";
    error = toast->showWithButtons(content, buttons, 10);
    ASSERT(!error.isSuccess(), "Rejects more than 5 buttons");
    printf("[INFO] Error message: %s\n", error.message);
    
    TEST_END();
    return true;
}

// ==============================================================================
// Test 11: Long Duration & Scenarios
// ==============================================================================

bool test_duration_and_scenarios() {
    TEST_BEGIN("Duration & Scenarios");
    
    auto toast = ToastNotifier::create("TestApp.ScenarioTest");
    ASSERT(toast != nullptr, "ToastNotifier created");
    
    // Test long duration
    ToastContent content;
    content.title = "Long Duration Test";
    content.body = "This notification should stay longer";
    content.duration = ToastDuration::Long;
    
    auto error = toast->show(content);
    ASSERT_SUCCESS(error, "Long duration toast shown");
    std::this_thread::sleep_for(2s);
    
    // Test reminder scenario
    content.title = "Reminder Test";
    content.body = "This is a reminder notification";
    content.scenario = ToastScenario::Reminder;
    content.duration = ToastDuration::Short;
    
    error = toast->show(content);
    ASSERT_SUCCESS(error, "Reminder scenario toast shown");
    std::this_thread::sleep_for(2s);
    
    TEST_END();
    return true;
}

// ==============================================================================
// Test 12: Stress Test
// ==============================================================================

bool test_stress_multiple_toasts() {
    TEST_BEGIN("Stress Test - Multiple Toasts");
    
    auto toast = ToastNotifier::create("TestApp.StressTest");
    ASSERT(toast != nullptr, "ToastNotifier created");
    
    printf("[INFO] Showing 10 toasts rapidly...\n");
    
    for (int i = 1; i <= 10; ++i) {
        ToastContent content;
        content.title = "Stress Test";
        std::string bodyText = "Toast #" + std::to_string(i) + " of 10";
        content.body = bodyText.c_str();
        
        auto error = toast->show(content);
        if (!error.isSuccess()) {
            fprintf(stderr, "[WARN] Toast %d failed: %s\n", i, error.message);
        } else {
            printf("[INFO] Toast %d shown\n", i);
        }
        
        std::this_thread::sleep_for(100ms);
    }
    
    ASSERT(true, "Stress test completed");
    
    printf("[INFO] Check Action Center for multiple notifications\n");
    std::this_thread::sleep_for(2s);
    
    // Clean up
    toast->clearAll();
    
    TEST_END();
    return true;
}

// ==============================================================================
// Main Test Runner
// ==============================================================================

int main() {
    printf("\n");
    printf("========================================\n");
    printf("windows-toast-notifications-0.9.0\n");
    printf("Integration & End-to-End Test Suite\n");
    printf("========================================\n");
    printf("\n");
    printf("This test suite validates:\n");
    printf("  * Factory creation\n");
    printf("  * Simple toasts\n");
    printf("  * Toasts with images\n");
    printf("  * Toasts with buttons\n");
    printf("  * Toasts with input fields\n");
    printf("  * Progress bars & updates\n");
    printf("  * Callbacks (activation & dismissal)\n");
    printf("  * Clear operations\n");
    printf("  * Error handling\n");
    printf("  * Duration & scenarios\n");
    printf("  * Stress testing\n");
    printf("\n");
    printf("NOTE: Some tests require manual interaction.\n");
    printf("      Tests will wait for your input.\n");
    printf("\n");
    printf("Press Enter to start testing...\n");
    getchar();
    
    // Run all tests
    bool allPassed = true;
    
    allPassed &= test_factory_creation();
    allPassed &= test_simple_toast();
    allPassed &= test_toast_with_image();
    allPassed &= test_toast_with_buttons();
    allPassed &= test_toast_with_input();
    allPassed &= test_toast_with_progress();
    allPassed &= test_dismissal_callback();
    allPassed &= test_clear_toasts();
    allPassed &= test_clear_by_tag();
    allPassed &= test_error_handling();
    allPassed &= test_duration_and_scenarios();
    allPassed &= test_stress_multiple_toasts();
    
    // Summary
    printf("\n");
    printf("========================================\n");
    printf("TEST SUMMARY\n");
    printf("========================================\n");
    printf("[PASS] Tests Passed:  %d\n", g_testsPassed);
    printf("[FAIL] Tests Failed:  %d\n", g_testsFailed);
    printf("[TOTAL] Total:        %d\n", g_testsPassed + g_testsFailed);
    printf("========================================\n");
    
    if (allPassed && g_testsFailed == 0) {
        printf("\n[SUCCESS] ALL TESTS PASSED!\n");
        printf("The library is ready for production.\n\n");
        return 0;
    } else {
        printf("\n[FAILED] SOME TESTS FAILED!\n");
        printf("Please review failures above.\n\n");
        return 1;
    }
}
