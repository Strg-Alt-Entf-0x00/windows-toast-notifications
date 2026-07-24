// ==============================================================================
// test_winrt_direct.cpp - Direct WinRT Test (Bypass Library)
// ==============================================================================

#include <Windows.h>
#include <shobjidl.h>  // For SetCurrentProcessExplicitAppUserModelID
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Notifications.h>
#include <winrt/Windows.Data.Xml.Dom.h>
#include <cstdio>

using namespace winrt;
using namespace winrt::Windows::UI::Notifications;
using namespace winrt::Windows::Data::Xml::Dom;

int main() {
    printf("\n");
    printf("========================================\n");
    printf("Direct WinRT Toast Test\n");
    printf("========================================\n");
    printf("\n");
    
    try {
        // Step 1: Initialize WinRT
        printf("[1/4] Initializing WinRT...\n");
        try {
            init_apartment(apartment_type::multi_threaded);
            printf("      [OK] init_apartment succeeded\n");
        }
        catch (const hresult_error& ex) {
            if (ex.code() == hresult(0x80010106)) {
                printf("      [OK] Apartment already initialized (RPC_E_CHANGED_MODE)\n");
            } else {
                printf("      [ERROR] init_apartment failed: 0x%08X\n", static_cast<int32_t>(ex.code()));
                printf("              %ls\n", ex.message().c_str());
                return 1;
            }
        }
        
        printf("\n");
        
        // Step 2: Register AppUserModelID
        printf("[2/4] Registering AppUserModelID...\n");
        const wchar_t* appId = L"WinRTDirectTest.ToastNotifications";
        HRESULT hr = SetCurrentProcessExplicitAppUserModelID(appId);
        
        if (SUCCEEDED(hr)) {
            printf("      [OK] SetCurrentProcessExplicitAppUserModelID succeeded\n");
        } else {
            printf("      [ERROR] SetCurrentProcessExplicitAppUserModelID failed: 0x%08X\n", hr);
            return 1;
        }
        
        printf("\n");
        
        // Step 3: Create ToastNotifier
        printf("[3/4] Creating ToastNotifier...\n");
        ToastNotifier notifier = nullptr;
        
        try {
            notifier = ToastNotificationManager::CreateToastNotifier(appId);
            printf("      [OK] CreateToastNotifier succeeded\n");
        }
        catch (const hresult_error& ex) {
            printf("      [ERROR] CreateToastNotifier failed: 0x%08X\n", static_cast<int32_t>(ex.code()));
            printf("              %ls\n", ex.message().c_str());
            return 1;
        }
        
        printf("\n");
        
        // Step 4: Create and show toast
        printf("[4/4] Creating and showing toast...\n");
        
        // Simple toast XML
        std::wstring xmlContent = LR"(
            <toast>
                <visual>
                    <binding template="ToastGeneric">
                        <text>WinRT Direct Test</text>
                        <text>If you see this, WinRT is working!</text>
                    </binding>
                </visual>
            </toast>
        )";
        
        try {
            XmlDocument xmlDoc;
            xmlDoc.LoadXml(xmlContent);
            
            ToastNotification toast(xmlDoc);
            notifier.Show(toast);
            
            printf("      [OK] Toast.Show() succeeded\n");
            printf("\n");
            printf("========================================\n");
            printf("[SUCCESS] Toast was shown!\n");
            printf("========================================\n");
            printf("\n");
            printf("Check your notification center.\n");
            printf("You should see: 'WinRT Direct Test'\n");
            printf("\n");
            
            return 0;
        }
        catch (const hresult_error& ex) {
            printf("      [ERROR] Toast creation/show failed: 0x%08X\n", static_cast<int32_t>(ex.code()));
            printf("              %ls\n", ex.message().c_str());
            return 1;
        }
    }
    catch (...) {
        printf("[ERROR] Unexpected exception\n");
        return 1;
    }
}
