// ==============================================================================
// test_unit.cpp - Unit Tests (Automated, No User Interaction)
// ==============================================================================
// Description: Automated unit tests that can run in CI/CD
// Purpose:     Fast validation without manual intervention
// ==============================================================================

#include <toast_notifications/toast_notification.h>
#include <toast_notifications/types.h>
#include <toast_notifications/error.h>
#include <cstdio>
#include <cassert>

using namespace windows_platform;

// ==============================================================================
// Test Framework
// ==============================================================================

static int g_testsPassed = 0;
static int g_testsFailed = 0;

#define UNIT_TEST(name) \
    static bool unit_test_##name(); \
    static bool run_##name() { \
        printf("[TEST] %s ... ", #name); \
        fflush(stdout); \
        bool result = unit_test_##name(); \
        if (result) { \
            printf("[PASS]\n"); \
            g_testsPassed++; \
        } else { \
            printf("[FAIL]\n"); \
            g_testsFailed++; \
        } \
        return result; \
    } \
    static bool unit_test_##name()

#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        fprintf(stderr, "  Assertion failed: %s\n", #condition); \
        return false; \
    }

#define ASSERT_FALSE(condition) \
    if (condition) { \
        fprintf(stderr, "  Assertion failed: NOT %s\n", #condition); \
        return false; \
    }

#define ASSERT_NULL(ptr) ASSERT_TRUE((ptr) == nullptr)
#define ASSERT_NOT_NULL(ptr) ASSERT_TRUE((ptr) != nullptr)
#define ASSERT_SUCCESS(error) ASSERT_TRUE((error).isSuccess())
#define ASSERT_FAILURE(error) ASSERT_FALSE((error).isSuccess())

// ==============================================================================
// Unit Tests
// ==============================================================================

UNIT_TEST(factory_creates_valid_instance) {
    auto toast = ToastNotifier::create("UnitTest.Factory");
    ASSERT_NOT_NULL(toast);
    return true;
}

UNIT_TEST(factory_handles_default_appid) {
    auto toast = ToastNotifier::create();
    ASSERT_NOT_NULL(toast);
    return true;
}

UNIT_TEST(factory_handles_custom_appid) {
    auto toast = ToastNotifier::create("MyCustomApp.Testing.123");
    ASSERT_NOT_NULL(toast);
    return true;
}

UNIT_TEST(error_info_success_state) {
    ErrorInfo success = Errors::Success;
    ASSERT_TRUE(success.isSuccess());
    ASSERT_TRUE(success.code == 0);
    return true;
}

UNIT_TEST(error_info_failure_state) {
    ErrorInfo error = Errors::NotInitialized;
    ASSERT_FALSE(error.isSuccess());
    ASSERT_TRUE(error.code != 0);
    ASSERT_NOT_NULL(error.message);
    return true;
}

UNIT_TEST(toast_content_initialization) {
    ToastContent content;
    content.title = "Test Title";
    content.body = "Test Body";
    content.attribution = "Test App";
    content.imagePath = "file:///C:/test.png";
    content.duration = ToastDuration::Long;
    content.audio = ToastAudio::Silent;
    content.scenario = ToastScenario::Reminder;
    content.imagePlacement = ToastImagePlacement::Hero;
    
    ASSERT_NOT_NULL(content.title);
    ASSERT_NOT_NULL(content.body);
    ASSERT_TRUE(content.duration == ToastDuration::Long);
    ASSERT_TRUE(content.audio == ToastAudio::Silent);
    return true;
}

UNIT_TEST(toast_button_initialization) {
    ToastButton button;
    button.label = "Click Me";
    button.arguments = "action=test";
    button.iconPath = "file:///C:/icon.png";
    
    ASSERT_NOT_NULL(button.label);
    ASSERT_NOT_NULL(button.arguments);
    ASSERT_NOT_NULL(button.iconPath);
    return true;
}

UNIT_TEST(toast_input_initialization) {
    ToastInput input;
    input.id = "inputBox";
    input.placeholder = "Enter text...";
    input.defaultValue = "Default";
    
    ASSERT_NOT_NULL(input.id);
    ASSERT_NOT_NULL(input.placeholder);
    ASSERT_NOT_NULL(input.defaultValue);
    return true;
}

UNIT_TEST(toast_progress_initialization) {
    ToastProgress progress;
    progress.title = "Downloading";
    progress.value = 0.75;
    progress.valueString = "75%";
    progress.status = "In progress";
    
    ASSERT_NOT_NULL(progress.title);
    ASSERT_TRUE(progress.value >= 0.0 && progress.value <= 1.0);
    ASSERT_NOT_NULL(progress.valueString);
    return true;
}

UNIT_TEST(show_requires_title) {
    auto toast = ToastNotifier::create("UnitTest.ShowRequiresTitle");
    ASSERT_NOT_NULL(toast);
    
    ToastContent content;
    content.title = nullptr;  // Invalid
    content.body = "Test body";
    
    auto error = toast->show(content);
    ASSERT_FAILURE(error);  // Should fail
    return true;
}

UNIT_TEST(show_accepts_null_body) {
    auto toast = ToastNotifier::create("UnitTest.NullBody");
    ASSERT_NOT_NULL(toast);
    
    ToastContent content;
    content.title = "Valid Title";
    content.body = nullptr;  // Optional
    
    auto error = toast->show(content);
    ASSERT_SUCCESS(error);  // Should succeed
    return true;
}

UNIT_TEST(show_with_buttons_rejects_too_many) {
    auto toast = ToastNotifier::create("UnitTest.TooManyButtons");
    ASSERT_NOT_NULL(toast);
    
    ToastButton buttons[10];
    for (int i = 0; i < 10; ++i) {
        buttons[i].label = "Button";
        buttons[i].arguments = "test";
    }
    
    ToastContent content;
    content.title = "Button Test";
    
    auto error = toast->showWithButtons(content, buttons, 10);
    ASSERT_FAILURE(error);  // Should fail (max 5 buttons)
    return true;
}

UNIT_TEST(show_with_buttons_accepts_valid_count) {
    auto toast = ToastNotifier::create("UnitTest.ValidButtonCount");
    ASSERT_NOT_NULL(toast);
    
    ToastButton buttons[3];
    for (int i = 0; i < 3; ++i) {
        buttons[i].label = "Button";
        buttons[i].arguments = "test";
    }
    
    ToastContent content;
    content.title = "Button Test";
    
    auto error = toast->showWithButtons(content, buttons, 3);
    ASSERT_SUCCESS(error);  // Should succeed
    return true;
}

UNIT_TEST(show_with_input_requires_input_id) {
    auto toast = ToastNotifier::create("UnitTest.InputRequiresId");
    ASSERT_NOT_NULL(toast);
    
    ToastInput input;
    input.id = nullptr;  // Invalid
    input.placeholder = "Test";
    
    ToastContent content;
    content.title = "Input Test";
    
    auto error = toast->showWithInput(content, input, nullptr, 0);
    ASSERT_FAILURE(error);  // Should fail
    return true;
}

UNIT_TEST(show_with_progress_requires_tag) {
    auto toast = ToastNotifier::create("UnitTest.ProgressRequiresTag");
    ASSERT_NOT_NULL(toast);
    
    ToastContent content;
    content.title = "Progress Test";
    
    ToastProgress progress;
    progress.title = "Processing";
    progress.value = 0.5;
    
    auto error = toast->showWithProgress(content, progress, nullptr);
    ASSERT_FAILURE(error);  // Should fail (null tag)
    return true;
}

UNIT_TEST(callbacks_can_be_set) {
    auto toast = ToastNotifier::create("UnitTest.Callbacks");
    ASSERT_NOT_NULL(toast);
    
    bool activated = false;
    bool dismissed = false;
    
    toast->onActivated([&](const char* /*args*/, const char* /*input*/) {
        activated = true;
    });
    
    toast->onDismissed([&]() {
        dismissed = true;
    });
    
    // Callbacks set successfully (no crash)
    ASSERT_FALSE(activated);  // Not invoked yet
    ASSERT_FALSE(dismissed);  // Not invoked yet
    return true;
}

UNIT_TEST(clear_all_succeeds) {
    auto toast = ToastNotifier::create("UnitTest.ClearAll");
    ASSERT_NOT_NULL(toast);
    
    auto error = toast->clearAll();
    ASSERT_SUCCESS(error);
    return true;
}

UNIT_TEST(clear_by_tag_requires_tag) {
    auto toast = ToastNotifier::create("UnitTest.ClearByTag");
    ASSERT_NOT_NULL(toast);
    
    auto error = toast->clearByTag(nullptr);
    ASSERT_FAILURE(error);  // Should fail
    return true;
}

UNIT_TEST(update_progress_requires_tag) {
    auto toast = ToastNotifier::create("UnitTest.UpdateProgressTag");
    ASSERT_NOT_NULL(toast);
    
    ToastProgress progress;
    progress.value = 0.8;
    
    auto error = toast->updateProgress(nullptr, progress);
    ASSERT_FAILURE(error);  // Should fail
    return true;
}

UNIT_TEST(toast_duration_enum_values) {
    const bool different = (ToastDuration::Short != ToastDuration::Long);
    ASSERT_TRUE(different);
    return true;
}

UNIT_TEST(toast_audio_enum_values) {
    const bool different1 = (ToastAudio::Default != ToastAudio::Silent);
    const bool different2 = (ToastAudio::Silent != ToastAudio::Loop);
    ASSERT_TRUE(different1);
    ASSERT_TRUE(different2);
    return true;
}

UNIT_TEST(toast_scenario_enum_values) {
    const bool diff1 = (ToastScenario::Default != ToastScenario::Reminder);
    const bool diff2 = (ToastScenario::Reminder != ToastScenario::Alarm);
    const bool diff3 = (ToastScenario::Alarm != ToastScenario::IncomingCall);
    ASSERT_TRUE(diff1);
    ASSERT_TRUE(diff2);
    ASSERT_TRUE(diff3);
    return true;
}

UNIT_TEST(toast_image_placement_enum_values) {
    const bool diff1 = (ToastImagePlacement::Inline != ToastImagePlacement::AppLogo);
    const bool diff2 = (ToastImagePlacement::AppLogo != ToastImagePlacement::Hero);
    ASSERT_TRUE(diff1);
    ASSERT_TRUE(diff2);
    return true;
}

UNIT_TEST(multiple_instances_can_coexist) {
    auto toast1 = ToastNotifier::create("UnitTest.Instance1");
    auto toast2 = ToastNotifier::create("UnitTest.Instance2");
    auto toast3 = ToastNotifier::create("UnitTest.Instance3");
    
    ASSERT_NOT_NULL(toast1);
    ASSERT_NOT_NULL(toast2);
    ASSERT_NOT_NULL(toast3);
    
    // All can operate independently
    ToastContent content;
    content.title = "Multi-instance Test";
    
    ASSERT_SUCCESS(toast1->show(content));
    ASSERT_SUCCESS(toast2->show(content));
    ASSERT_SUCCESS(toast3->show(content));
    
    return true;
}

// ==============================================================================
// Test Runner
// ==============================================================================

int main() {
    printf("\n");
    printf("========================================\n");
    printf("windows-toast-notifications-0.9.0\n");
    printf("Automated Unit Test Suite\n");
    printf("========================================\n");
    printf("\n");
    
    // Run all unit tests
    run_factory_creates_valid_instance();
    run_factory_handles_default_appid();
    run_factory_handles_custom_appid();
    run_error_info_success_state();
    run_error_info_failure_state();
    run_toast_content_initialization();
    run_toast_button_initialization();
    run_toast_input_initialization();
    run_toast_progress_initialization();
    run_show_requires_title();
    run_show_accepts_null_body();
    run_show_with_buttons_rejects_too_many();
    run_show_with_buttons_accepts_valid_count();
    run_show_with_input_requires_input_id();
    run_show_with_progress_requires_tag();
    run_callbacks_can_be_set();
    run_clear_all_succeeds();
    run_clear_by_tag_requires_tag();
    run_update_progress_requires_tag();
    run_toast_duration_enum_values();
    run_toast_audio_enum_values();
    run_toast_scenario_enum_values();
    run_toast_image_placement_enum_values();
    run_multiple_instances_can_coexist();
    
    // Summary
    printf("\n");
    printf("========================================\n");
    printf("TEST SUMMARY\n");
    printf("========================================\n");
    printf("[PASS] Passed:  %d\n", g_testsPassed);
    printf("[FAIL] Failed:  %d\n", g_testsFailed);
    printf("[TOTAL] Total: %d\n", g_testsPassed + g_testsFailed);
    printf("========================================\n");
    
    if (g_testsFailed == 0) {
        printf("\n[SUCCESS] ALL UNIT TESTS PASSED!\n\n");
        return 0;
    } else {
        printf("\n[FAILED] SOME UNIT TESTS FAILED!\n\n");
        return 1;
    }
}
