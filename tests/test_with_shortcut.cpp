// ==============================================================================
// test_with_shortcut.cpp - Test with Start Menu Shortcut Creation
// ==============================================================================

#include <Windows.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <propkey.h>
#include <propvarutil.h>
#include <toast_notifications/toast_notification.h>
#include <cstdio>
#include <thread>
#include <chrono>

// Function to create a shortcut in Start Menu
bool createStartMenuShortcut(const wchar_t* appName, const wchar_t* appId) {
    printf("📁 Erstelle Start Menu Shortcut...\n");
    
    // Get Start Menu path
    wchar_t startMenuPath[MAX_PATH];
    if (FAILED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, startMenuPath))) {
        printf("   ❌ Fehler: Kann Start Menu Pfad nicht finden\n");
        return false;
    }
    
    wcscat_s(startMenuPath, L"\\Microsoft\\Windows\\Start Menu\\Programs\\");
    wcscat_s(startMenuPath, appName);
    wcscat_s(startMenuPath, L".lnk");
    
    printf("   Pfad: %ls\n", startMenuPath);
    
    // Get current executable path
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    
    // Create shortcut using IShellLink
    IShellLinkW* pShellLink = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                                   IID_IShellLinkW, (void**)&pShellLink);
    
    if (FAILED(hr)) {
        printf("   ❌ Fehler: CoCreateInstance failed\n");
        return false;
    }
    
    pShellLink->SetPath(exePath);
    pShellLink->SetDescription(appName);
    
    // Set AppUserModelID
    IPropertyStore* pPropStore = nullptr;
    hr = pShellLink->QueryInterface(IID_IPropertyStore, (void**)&pPropStore);
    
    if (SUCCEEDED(hr)) {
        PROPVARIANT pv;
        hr = InitPropVariantFromString(appId, &pv);
        
        if (SUCCEEDED(hr)) {
            hr = pPropStore->SetValue(PKEY_AppUserModel_ID, pv);
            pPropStore->Commit();
            PropVariantClear(&pv);
        }
        
        pPropStore->Release();
    }
    
    // Save shortcut
    IPersistFile* pPersistFile = nullptr;
    hr = pShellLink->QueryInterface(IID_IPersistFile, (void**)&pPersistFile);
    
    if (SUCCEEDED(hr)) {
        hr = pPersistFile->Save(startMenuPath, TRUE);
        pPersistFile->Release();
    }
    
    pShellLink->Release();
    
    if (SUCCEEDED(hr)) {
        printf("   ✅ Shortcut erstellt!\n");
        return true;
    } else {
        printf("   ❌ Fehler beim Speichern: 0x%08X\n", hr);
        return false;
    }
}

int main() {
    printf("\n");
    printf("========================================\n");
    printf("Toast Test MIT Start Menu Shortcut\n");
    printf("========================================\n\n");
    
    CoInitialize(NULL);
    
    const wchar_t* appName = L"ToastTestApp";
    const wchar_t* appId = L"ToastTest.WithShortcut.v1";
    
    // Step 1: Create shortcut
    printf("[1/3] Shortcut-Erstellung...\n");
    if (!createStartMenuShortcut(appName, appId)) {
        printf("\n⚠️  Shortcut konnte nicht erstellt werden.\n");
        printf("   Toast wird trotzdem versucht...\n\n");
    } else {
        printf("\n✅ Shortcut erfolgreich erstellt!\n");
        printf("   (Du kannst ihn im Startmenü finden)\n\n");
    }
    
    // Step 2: Create toast notifier
    printf("[2/3] Toast Notifier erstellen...\n");
    auto toast = windows_platform::ToastNotifier::create("ToastTest.WithShortcut.v1");
    
    if (!toast) {
        printf("   ❌ Fehler: ToastNotifier konnte nicht erstellt werden!\n");
        CoUninitialize();
        return 1;
    }
    
    printf("   ✅ ToastNotifier erstellt\n\n");
    
    // Step 3: Send toast
    printf("[3/3] Toast senden...\n");
    
    windows_platform::ToastContent content;
    content.title = "🎉 TEST MIT SHORTCUT!";
    content.body = "Wenn du das siehst, funktioniert es!\nDrücke Win+A für Action Center";
    content.duration = windows_platform::ToastDuration::Long;
    content.scenario = windows_platform::ToastScenario::Reminder;
    
    auto result = toast->show(content);
    
    if (result.isSuccess()) {
        printf("   ✅ Toast gesendet!\n\n");
    } else {
        printf("   ❌ Fehler: %s\n\n", result.message);
        CoUninitialize();
        return 1;
    }
    
    printf("========================================\n");
    printf("ERFOLGREICH!\n");
    printf("========================================\n\n");
    
    printf("📋 WAS JETZT:\n\n");
    printf("1. Drücke Windows-Taste + A\n");
    printf("2. Schau im Action Center\n");
    printf("3. Du solltest die Toast sehen!\n\n");
    
    printf("💡 HINWEIS:\n");
    printf("Mit dem Shortcut im Startmenü sollte\n");
    printf("Windows die App jetzt erkennen und\n");
    printf("Toasts anzeigen!\n\n");
    
    printf("Warte 5 Sekunden...\n");
    std::this_thread::sleep_for(std::chrono::seconds(5));
    
    printf("\n========================================\n\n");
    
    CoUninitialize();
    return 0;
}
