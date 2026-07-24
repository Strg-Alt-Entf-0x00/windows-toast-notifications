// ==============================================================================
// test_debug_factory.cpp - Debug Factory Creation
// ==============================================================================

#include "../src/appid_manager.h"
#include <cstdio>
#include <Windows.h>

using namespace windows_platform;

int main() {
    printf("\n");
    printf("========================================\n");
    printf("Factory Debug Test\n");
    printf("========================================\n");
    printf("\n");
    
    // Step 1: Check isSupported()
    printf("[1/2] Testing AppIdManager::isSupported()...\n");
    bool supported = AppIdManager::isSupported();
    printf("      Result: %s\n", supported ? "TRUE" : "FALSE");
    
    if (!supported) {
        printf("      [ERROR] isSupported() returned false!\n");
        return 1;
    }
    
    printf("      [OK] Windows version is supported\n");
    printf("\n");
    
    // Step 2: Check registerAppId()
    printf("[2/2] Testing AppIdManager::registerAppId()...\n");
    const char* testAppId = "FactoryDebugTest.ToastNotifications";
    bool registered = AppIdManager::registerAppId(testAppId);
    printf("      Result: %s\n", registered ? "TRUE" : "FALSE");
    
    if (!registered) {
        printf("      [ERROR] registerAppId() returned false!\n");
        return 1;
    }
    
    printf("      [OK] AppUserModelID registration succeeded\n");
    printf("\n");
    printf("========================================\n");
    printf("[SUCCESS] Both checks passed!\n");
    printf("========================================\n");
    printf("\n");
    printf("The factory should work. If ToastNotifier::create()\n");
    printf("still returns nullptr, the problem is in Impl::initialize().\n");
    printf("\n");
    
    return 0;
}
