// ==============================================================================
// appid_manager.cpp - AppUserModelID Registration Implementation
// ==============================================================================

#include "appid_manager.h"
#include <Windows.h>
#include <shobjidl.h>
#include <VersionHelpers.h>
#include <propkey.h>
#include <propvarutil.h>

namespace windows_platform {

// ==============================================================================
// OS Version Check
// ==============================================================================

bool AppIdManager::isSupported() {
    // Requires Windows 10 Build 10240+ (TH1 / Anniversary Update)
    // Released: July 2015
    
    // IsWindows10OrGreater() requires proper manifest, so use RtlGetVersion instead
    typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);
    
    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (!ntdll) {
        // Fallback: assume supported (Windows 10+)
        return true;
    }
    
    auto RtlGetVersion = (RtlGetVersionPtr)GetProcAddress(ntdll, "RtlGetVersion");
    if (!RtlGetVersion) {
        // Fallback: assume supported
        return true;
    }
    
    RTL_OSVERSIONINFOW osvi = {};
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    
    if (RtlGetVersion(&osvi) == 0) {
        // Windows 10 is version 10.0.10240 or higher
        if (osvi.dwMajorVersion > 10) {
            return true;  // Windows 11+
        }
        if (osvi.dwMajorVersion == 10 && osvi.dwBuildNumber >= 10240) {
            return true;  // Windows 10 Build 10240+
        }
        return false;  // Older than Windows 10
    }
    
    // Fallback: assume supported
    return true;
}

// ==============================================================================
// AppUserModelID Registration
// ==============================================================================

bool AppIdManager::registerAppId(const char* appId) {
    if (!appId || strlen(appId) == 0) {
        return false;
    }
    
    // Convert to wide string
    int size = MultiByteToWideChar(CP_UTF8, 0, appId, -1, nullptr, 0);
    if (size <= 0) {
        return false;
    }
    
    std::wstring wideAppId(size - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, appId, -1, wideAppId.data(), size);
    
    // Register AppUserModelID for current process
    // This tells Windows which app is showing toasts
    HRESULT hr = SetCurrentProcessExplicitAppUserModelID(wideAppId.c_str());
    
    return SUCCEEDED(hr);
}

// ==============================================================================
// Query Current AppUserModelID
// ==============================================================================

std::string AppIdManager::getCurrentAppId() {
    PWSTR appIdWide = nullptr;
    
    HRESULT hr = GetCurrentProcessExplicitAppUserModelID(&appIdWide);
    
    if (FAILED(hr) || !appIdWide) {
        return "";
    }
    
    // Convert to narrow string
    int size = WideCharToMultiByte(CP_UTF8, 0, appIdWide, -1, nullptr, 0, nullptr, nullptr);
    if (size <= 0) {
        CoTaskMemFree(appIdWide);
        return "";
    }
    
    std::string result(size - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, appIdWide, -1, result.data(), size, nullptr, nullptr);
    
    CoTaskMemFree(appIdWide);
    
    return result;
}

} // namespace windows_platform
