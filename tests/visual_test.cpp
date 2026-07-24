// ==============================================================================
// visual_test.cpp - Visual Confirmation Test
// ==============================================================================

#include <toast_notifications/toast_notification.h>
#include <cstdio>
#include <thread>
#include <chrono>

int main() {
    printf("\n");
    printf("========================================\n");
    printf("  VISUAL TEST - Toast Notifications\n");
    printf("========================================\n\n");
    
    printf("Dieser Test zeigt 3 Toast-Benachrichtigungen.\n");
    printf("Du solltest sie in deinem Action Center sehen!\n\n");
    
    auto toast = windows_platform::ToastNotifier::create("VisualTest.ToastNotifications");
    
    if (!toast) {
        fprintf(stderr, "❌ FEHLER: ToastNotifier konnte nicht erstellt werden!\n");
        return 1;
    }
    
    printf("✅ ToastNotifier erstellt\n\n");
    
    // Test 1: Einfache Toast
    printf("[Test 1/3] Zeige einfache Toast-Benachrichtigung...\n");
    windows_platform::ToastContent content1;
    content1.title = "🎉 TEST 1: Einfache Toast";
    content1.body = "Wenn du das siehst, funktioniert die Library!";
    
    auto result1 = toast->show(content1);
    if (result1.isSuccess()) {
        printf("   ✅ Toast 1 gesendet\n\n");
    } else {
        printf("   ❌ Fehler: %s\n\n", result1.message);
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Test 2: Toast mit mehr Info
    printf("[Test 2/3] Zeige Toast mit ausführlicher Info...\n");
    windows_platform::ToastContent content2;
    content2.title = "🔔 TEST 2: Erweiterte Toast";
    content2.body = "Dies ist eine längere Nachricht.\nMit mehreren Zeilen.\nWindows Toast Notifications funktioniert! ✅";
    content2.duration = windows_platform::ToastDuration::Long;
    
    auto result2 = toast->show(content2);
    if (result2.isSuccess()) {
        printf("   ✅ Toast 2 gesendet\n\n");
    } else {
        printf("   ❌ Fehler: %s\n\n", result2.message);
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(2));
    
    // Test 3: Toast mit Buttons
    printf("[Test 3/3] Zeige Toast mit Action-Buttons...\n");
    windows_platform::ToastContent content3;
    content3.title = "⭐ TEST 3: Interactive Toast";
    content3.body = "Klicke auf einen Button!";
    
    windows_platform::ToastButton buttons[] = {
        { "👍 Funktioniert!", "action=works" },
        { "❌ Fehler", "action=error" }
    };
    
    bool buttonClicked = false;
    toast->onActivated([&](const char* args, const char*) {
        printf("\n   🎯 Button wurde geklickt: %s\n", args);
        buttonClicked = true;
    });
    
    auto result3 = toast->showWithButtons(content3, buttons, 2);
    if (result3.isSuccess()) {
        printf("   ✅ Toast 3 gesendet (mit Buttons)\n\n");
    } else {
        printf("   ❌ Fehler: %s\n\n", result3.message);
    }
    
    // Warte auf Button-Klick
    printf("Warte 10 Sekunden auf Button-Klick...\n");
    for (int i = 0; i < 10; i++) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (buttonClicked) {
            printf("   ✅ Button-Callback funktioniert!\n");
            break;
        }
        printf("   ⏳ %d...\n", 10 - i);
    }
    
    printf("\n");
    printf("========================================\n");
    printf("  TEST ABGESCHLOSSEN\n");
    printf("========================================\n\n");
    
    printf("📋 ZUSAMMENFASSUNG:\n\n");
    printf("Du solltest in deinem Windows Action Center\n");
    printf("(Rechts unten in der Taskleiste, Glocken-Symbol)\n");
    printf("3 Toast-Benachrichtigungen sehen:\n\n");
    printf("  1️⃣  'TEST 1: Einfache Toast'\n");
    printf("  2️⃣  'TEST 2: Erweiterte Toast'\n");
    printf("  3️⃣  'TEST 3: Interactive Toast' (mit Buttons)\n\n");
    
    printf("📌 WIE MAN DAS ACTION CENTER ÖFFNET:\n");
    printf("   • Windows-Taste + A drücken\n");
    printf("   • ODER: Rechts unten auf die Uhrzeit klicken\n");
    printf("   • ODER: Auf das Glocken-Symbol klicken\n\n");
    
    if (buttonClicked) {
        printf("✅ Button-Test: ERFOLGREICH (Button wurde geklickt)\n");
    } else {
        printf("⚠️  Button-Test: NICHT GETESTET (kein Button geklickt)\n");
        printf("   Du kannst das Action Center öffnen und nachträglich klicken!\n");
    }
    
    printf("\n========================================\n\n");
    
    return 0;
}
