// ==============================================================================
// shortcut_manager.cpp - Start Menu Shortcut Management Implementation
// ==============================================================================

#include <toast_notifications/shortcut_manager.h>
#include <Windows.h>
#include <shobjidl.h>
#include <shlobj.h>
#include <propkey.h>
#include <propvarutil.h>
#include <string>

namespace windows_platform {

// Helper to convert narrow to wide string
static std::wstring toWide(const char* str) {
    if (!str) return L"";
    
    int size = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
    if (size <= 0) return L"";
    
    std::wstring result(size - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, str, -1, result.data(), size);
    
    return result;
}

// Helper to convert wide to narrow string
static std::string toNarrow(const wchar_t* wstr) {
    if (!wstr) return "";
    
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, nullptr, 0, nullptr, nullptr);
    if (size <= 0) return "";
    
    std::string result(size - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, result.data(), size, nullptr, nullptr);
    
    return result;
}

// ==============================================================================
// Public API Implementation
// ==============================================================================

ErrorInfo ShortcutManager::createShortcut(
    const char* appName,
    const char* appId,
    const char* description
) {
    if (!appName || !appId) {
        return Errors::InvalidParameter;
    }
    
    // Initialize COM
    HRESULT hr = CoInitialize(NULL);
    bool comInitialized = SUCCEEDED(hr);
    
    // Get Start Menu path
    wchar_t startMenuPath[MAX_PATH];
    hr = SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, startMenuPath);
    
    if (FAILED(hr)) {
        if (comInitialized) CoUninitialize();
        return ErrorInfo{ -1, "Failed to get Start Menu path" };
    }
    
    // Build shortcut path
    std::wstring shortcutPath = startMenuPath;
    shortcutPath += L"\\Microsoft\\Windows\\Start Menu\\Programs\\";
    shortcutPath += toWide(appName);
    shortcutPath += L".lnk";
    
    // Get current executable path
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    
    // Create shortcut using IShellLink
    IShellLinkW* pShellLink = nullptr;
    hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                         IID_IShellLinkW, (void**)&pShellLink);
    
    if (FAILED(hr)) {
        if (comInitialized) CoUninitialize();
        return ErrorInfo{ (int)hr, "Failed to create IShellLink instance" };
    }
    
    // Set shortcut properties
    pShellLink->SetPath(exePath);
    
    if (description) {
        pShellLink->SetDescription(toWide(description).c_str());
    } else {
        pShellLink->SetDescription(toWide(appName).c_str());
    }
    
    // Set AppUserModelID property
    IPropertyStore* pPropStore = nullptr;
    hr = pShellLink->QueryInterface(IID_IPropertyStore, (void**)&pPropStore);
    
    if (SUCCEEDED(hr)) {
        PROPVARIANT pv;
        hr = InitPropVariantFromString(toWide(appId).c_str(), &pv);
        
        if (SUCCEEDED(hr)) {
            hr = pPropStore->SetValue(PKEY_AppUserModel_ID, pv);
            pPropStore->Commit();
            PropVariantClear(&pv);
        }
        
        pPropStore->Release();
    }
    
    // Save shortcut to file
    IPersistFile* pPersistFile = nullptr;
    hr = pShellLink->QueryInterface(IID_IPersistFile, (void**)&pPersistFile);
    
    ErrorInfo result = Errors::Success;
    
    if (SUCCEEDED(hr)) {
        hr = pPersistFile->Save(shortcutPath.c_str(), TRUE);
        
        if (FAILED(hr)) {
            result = ErrorInfo{ (int)hr, "Failed to save shortcut file" };
        }
        
        pPersistFile->Release();
    } else {
        result = ErrorInfo{ (int)hr, "Failed to get IPersistFile interface" };
    }
    
    pShellLink->Release();
    
    if (comInitialized) {
        CoUninitialize();
    }
    
    return result;
}

bool ShortcutManager::shortcutExists(const char* appName) {
    if (!appName) return false;
    
    wchar_t startMenuPath[MAX_PATH];
    HRESULT hr = SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, startMenuPath);
    
    if (FAILED(hr)) return false;
    
    std::wstring shortcutPath = startMenuPath;
    shortcutPath += L"\\Microsoft\\Windows\\Start Menu\\Programs\\";
    shortcutPath += toWide(appName);
    shortcutPath += L".lnk";
    
    DWORD attribs = GetFileAttributesW(shortcutPath.c_str());
    return (attribs != INVALID_FILE_ATTRIBUTES);
}

ErrorInfo ShortcutManager::removeShortcut(const char* appName) {
    if (!appName) {
        return Errors::InvalidParameter;
    }
    
    wchar_t startMenuPath[MAX_PATH];
    HRESULT hr = SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, startMenuPath);
    
    if (FAILED(hr)) {
        return ErrorInfo{ -1, "Failed to get Start Menu path" };
    }
    
    std::wstring shortcutPath = startMenuPath;
    shortcutPath += L"\\Microsoft\\Windows\\Start Menu\\Programs\\";
    shortcutPath += toWide(appName);
    shortcutPath += L".lnk";
    
    if (DeleteFileW(shortcutPath.c_str())) {
        return Errors::Success;
    } else {
        DWORD error = GetLastError();
        if (error == ERROR_FILE_NOT_FOUND) {
            return Errors::Success; // Already deleted
        }
        return ErrorInfo{ (int)error, "Failed to delete shortcut" };
    }
}

std::string ShortcutManager::getShortcutPath(const char* appName) {
    if (!appName) return "";
    
    wchar_t startMenuPath[MAX_PATH];
    HRESULT hr = SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, startMenuPath);
    
    if (FAILED(hr)) return "";
    
    std::wstring shortcutPath = startMenuPath;
    shortcutPath += L"\\Microsoft\\Windows\\Start Menu\\Programs\\";
    shortcutPath += toWide(appName);
    shortcutPath += L".lnk";
    
    return toNarrow(shortcutPath.c_str());
}

} // namespace windows_platform
