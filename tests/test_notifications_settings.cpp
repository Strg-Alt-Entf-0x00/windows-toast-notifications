// ==============================================================================
// test_notifications_settings.cpp - Check Windows Notification Settings
// ==============================================================================

#include <Windows.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Notifications.h>
#include <cstdio>

int main() {
    printf("\n");
    printf("========================================\n");
    printf("Windows Benachrichtigungs-Einstellungen\n");
    printf("========================================\n\n");
    
    try {
        winrt::init_apartment();
        
        printf("[1/2] Prüfe Toast-Notifier-Status...\n");
        
        auto notifier = winrt::Windows::UI::Notifications::ToastNotificationManager::CreateToastNotifier(L"SettingsTest");
        auto setting = notifier.Setting();
        
        printf("      Status: ");
        
        using winrt::Windows::UI::Notifications::NotificationSetting;
        
        switch (setting) {
            case NotificationSetting::Enabled:
                printf("✅ AKTIVIERT\n");
                printf("      → Benachrichtigungen sind eingeschaltet!\n");
                printf("      → Die Library funktioniert korrekt!\n");
                break;
                
            case NotificationSetting::DisabledForApplication:
                printf("❌ DEAKTIVIERT FÜR DIESE APP\n");
                printf("      → Benachrichtigungen sind für Apps ausgeschaltet!\n");
                printf("\n");
                printf("      SO AKTIVIERST DU SIE:\n");
                printf("      1. Windows-Einstellungen öffnen (Win + I)\n");
                printf("      2. System → Benachrichtigungen\n");
                printf("      3. Suche nach der App und aktiviere sie\n");
                break;
                
            case NotificationSetting::DisabledForUser:
                printf("❌ DEAKTIVIERT FÜR BENUTZER\n");
                printf("      → ALLE Benachrichtigungen sind ausgeschaltet!\n");
                printf("\n");
                printf("      SO AKTIVIERST DU SIE:\n");
                printf("      1. Windows-Einstellungen öffnen (Win + I)\n");
                printf("      2. System → Benachrichtigungen\n");
                printf("      3. Schalter 'Benachrichtigungen' auf EIN\n");
                break;
                
            case NotificationSetting::DisabledByGroupPolicy:
                printf("❌ DURCH GRUPPENRICHTLINIE DEAKTIVIERT\n");
                printf("      → Benachrichtigungen sind durch Admin blockiert\n");
                break;
                
            case NotificationSetting::DisabledByManifest:
                printf("❌ DURCH APP-MANIFEST DEAKTIVIERT\n");
                break;
                
            default:
                printf("⚠️ UNBEKANNT: %d\n", (int)setting);
                break;
        }
        
        printf("\n");
        
        printf("[2/2] Zusätzliche Hinweise...\n\n");
        
        if (setting == NotificationSetting::Enabled) {
            printf("✅ Benachrichtigungen sind AKTIVIERT!\n");
            printf("   Falls du trotzdem nichts siehst:\n\n");
            printf("   • Fokusassistent prüfen (sollte AUS sein)\n");
            printf("   • Action Center direkt nach Toast öffnen (Win+A)\n");
            printf("   • Toast erscheint nur kurz, dann im Action Center\n");
        } else {
            printf("❌ Benachrichtigungen sind DEAKTIVIERT!\n");
            printf("   Bitte aktiviere sie zuerst.\n");
        }
        
    } catch (const winrt::hresult_error& ex) {
        printf("❌ FEHLER: 0x%08X\n", static_cast<int32_t>(ex.code()));
        return 1;
    }
    
    printf("\n");
    printf("========================================\n");
    printf("Windows-Einstellungen öffnen?\n");
    printf("========================================\n\n");
    printf("Befehl zum Öffnen:\n");
    printf("  start ms-settings:notifications\n\n");
    printf("Soll ich die Einstellungen öffnen? (y/n): ");
    
    int input = getchar();
    char c = static_cast<char>(input);
    if (c == 'y' || c == 'Y' || c == 'j' || c == 'J') {
        printf("\n✅ Öffne Windows-Einstellungen...\n");
        system("start ms-settings:notifications");
    }
    
    printf("\n========================================\n\n");
    
    return 0;
}
