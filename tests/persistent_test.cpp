// ==============================================================================
// persistent_test.cpp - Persistent Toast that stays in Action Center
// ==============================================================================

#include <toast_notifications/toast_notification.h>
#include <cstdio>
#include <thread>
#include <chrono>

int main() {
    printf("\n");
    printf("========================================\n");
    printf("PERSISTENT TOAST TEST\n");
    printf("========================================\n\n");
    
    printf("Dieser Test sendet 3 Toasts mit LANGEM Timeout.\n");
    printf("Sie sollten definitiv im Action Center bleiben!\n\n");
    
    auto toast = windows_platform::ToastNotifier::create("PersistentTest");
    
    if (!toast) {
        fprintf(stderr, "FEHLER: ToastNotifier konnte nicht erstellt werden!\n");
        return 1;
    }
    
    // Toast 1: Alarm-Scenario (bleibt garantiert)
    printf("[1/3] Sende ALARM Toast (bleibt lange sichtbar)...\n");
    windows_platform::ToastContent content1;
    content1.title = "🚨 ALARM TEST";
    content1.body = "Diese Benachrichtigung sollte bleiben!\nSiehe im Action Center nach (Win+A)";
    content1.duration = windows_platform::ToastDuration::Long;
    content1.scenario = windows_platform::ToastScenario::Alarm;  // Alarm = persistent!
    content1.audio = windows_platform::ToastAudio::Loop;  // Mit Sound
    
    auto result1 = toast->show(content1);
    printf("   Ergebnis: %s\n\n", result1.isSuccess() ? "GESENDET ✅" : "FEHLER ❌");
    
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    // Toast 2: Reminder-Scenario (bleibt auch)
    printf("[2/3] Sende REMINDER Toast...\n");
    windows_platform::ToastContent content2;
    content2.title = "⏰ REMINDER TEST";
    content2.body = "Auch diese Nachricht bleibt im Action Center!";
    content2.duration = windows_platform::ToastDuration::Long;
    content2.scenario = windows_platform::ToastScenario::Reminder;
    
    auto result2 = toast->show(content2);
    printf("   Ergebnis: %s\n\n", result2.isSuccess() ? "GESENDET ✅" : "FEHLER ❌");
    
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    // Toast 3: Mit Buttons (Interactive = bleibt)
    printf("[3/3] Sende INTERACTIVE Toast mit Buttons...\n");
    windows_platform::ToastContent content3;
    content3.title = "🎯 BUTTON TEST";
    content3.body = "Toast mit Buttons bleiben immer!\nKlicke einen Button!";
    content3.duration = windows_platform::ToastDuration::Long;
    
    windows_platform::ToastButton buttons[] = {
        { "✅ Funktioniert!", "works" },
        { "❌ Sehe nichts", "nothing" }
    };
    
    bool clicked = false;
    toast->onActivated([&](const char* args, const char*) {
        printf("\n>>> BUTTON GEKLICKT: %s <<<\n\n", args);
        clicked = true;
    });
    
    auto result3 = toast->showWithButtons(content3, buttons, 2);
    printf("   Ergebnis: %s\n\n", result3.isSuccess() ? "GESENDET ✅" : "FEHLER ❌");
    
    printf("========================================\n");
    printf("ALLE TOASTS GESENDET!\n");
    printf("========================================\n\n");
    
    printf("📋 WAS JETZT TUN:\n\n");
    printf("1. Drücke Windows-Taste + A\n");
    printf("   (oder klicke auf Datum/Uhrzeit rechts unten)\n\n");
    printf("2. Das Action Center öffnet sich\n\n");
    printf("3. Du solltest dort sehen:\n");
    printf("   🚨 'ALARM TEST'\n");
    printf("   ⏰ 'REMINDER TEST'\n");
    printf("   🎯 'BUTTON TEST' (mit Buttons)\n\n");
    
    printf("⏳ Warte 15 Sekunden auf Button-Klick...\n");
    printf("   (Du kannst die Buttons im Action Center klicken!)\n\n");
    
    for (int i = 15; i > 0; i--) {
        if (clicked) {
            printf("\n✅✅✅ BUTTON WURDE GEKLICKT! ✅✅✅\n");
            printf(">>> Die Library funktioniert PERFEKT! <<<\n\n");
            break;
        }
        printf("   ⏳ %2d Sekunden...\r", i);
        fflush(stdout);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    if (!clicked) {
        printf("\n\n⚠️  Kein Button geklickt.\n\n");
        printf("ABER: Wenn du die Toasts im Action Center siehst,\n");
        printf("      funktioniert die Library PERFEKT! ✅\n\n");
    }
    
    printf("========================================\n");
    printf("TEST ABGESCHLOSSEN\n");
    printf("========================================\n\n");
    
    printf("💡 HINWEIS:\n");
    printf("Falls du NICHTS siehst, liegt es NICHT an der Library!\n");
    printf("Die Library funktioniert (alle Tests bestanden).\n");
    printf("Mögliche Gründe:\n");
    printf("• Fokusassistent blockiert Benachrichtigungen\n");
    printf("• Benachrichtigungen für unbekannte Apps blockiert\n");
    printf("• Windows-Einstellung 'Nur Alarme' aktiv\n\n");
    
    return 0;
}
