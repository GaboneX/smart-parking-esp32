#define BLYNK_TEMPLATE_ID   "....."
#define BLYNK_TEMPLATE_NAME "....."
#define BLYNK_AUTH_TOKEN    "....."

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>

// Dane do hotspotu z iPhone'a
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "iPhone 15 Pro";
char pass[] = ".....";

// Piny RFID (SPI) dla ESP32
#define RST_PIN  25   
#define SS_PIN   22   
#define SCK_PIN  18   
#define MISO_PIN 19   
#define MOSI_PIN 23   

MFRC522 mfrc522(SS_PIN, RST_PIN);

// Piny peryferiów
#define SERVO_PIN      13   // Szlaban (serwo)
#define IR_BEFORE_GATE 32   // Czujnik przed bramą (zewnętrzny - do zamykania przy wyjeździe)
#define IR_AFTER_GATE  33   // Czujnik po bramie (wewnętrzny - do zamykania przy wjeździe / otwierania przy wyjeździe)

// Czujniki 3 miejsc parkingowych
#define SPOT_1_PIN     14
#define SPOT_2_PIN     26
#define SPOT_3_PIN     27

#define RED_LED_PIN     5   // Fizyczna czerwona dioda na płytce (pełny parking)

Servo szlabanServo;

// Uprawnione karty (4 sztuki)
String authorizedUIDs[] = {
  "8D AF 29 A9", 
  "0D 61 4D A6", 
  "BD A1 67 A9", 
  "DD 9A F0 A9"
};
const int totalUsers = 4;

bool waitingForCarToEnter = false; // Flaga: auto wjeżdża z zewnątrz
bool waitingForCarToExit = false;  // Flaga: auto wyjeżdża od środka

// Zmienne do zapamiętywania stanu miejsc (eliminacja spamu w konsoli i wysyłania do Blynka tylko przy zmianie)
bool lastSpot1 = false;
bool lastSpot2 = false;
bool lastSpot3 = false;

void otwórzSzlaban() {
  szlabanServo.write(90); // Kąt otwarcia
  Serial.println("🟢 [SZLABAN] Otwarty.");
}

void zamknijSzlaban() {
  szlabanServo.write(0);  // Kąt zamknięcia
  waitingForCarToEnter = false;
  waitingForCarToExit = false;
  Serial.println("🔴 [SZLABAN] Zamknięty.");
}

void setup() {
  Serial.begin(115200);
  
  pinMode(IR_BEFORE_GATE, INPUT_PULLUP);
  pinMode(IR_AFTER_GATE, INPUT_PULLUP);
  
  pinMode(SPOT_1_PIN, INPUT_PULLUP);
  pinMode(SPOT_2_PIN, INPUT_PULLUP);
  pinMode(SPOT_3_PIN, INPUT_PULLUP);
  
  pinMode(RED_LED_PIN, OUTPUT);
  digitalWrite(RED_LED_PIN, LOW);

  szlabanServo.attach(SERVO_PIN);
  zamknijSzlaban();

  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  mfrc522.PCD_Init();

  Serial.println("--- ŁĄCZENIE Z BLYNK (HOTSPOT IPHONE) ---");
  Blynk.begin(auth, ssid, pass);
  Serial.println("--- SMART PARKING URUCHOMIONY I POŁĄCZONY ---");
}

void loop() {
  Blynk.run(); // Obsługa połączenia Blynk

  // 1. Sprawdzenie fizycznego stanu miejsc (LOW = zajęte przez auto)
  bool spot1_busy = (digitalRead(SPOT_1_PIN) == LOW); 
  bool spot2_busy = (digitalRead(SPOT_2_PIN) == LOW);
  bool spot3_busy = (digitalRead(SPOT_3_PIN) == LOW);

  int zajeteMiejsca = 0;
  if (spot1_busy) zajeteMiejsca++;
  if (spot2_busy) zajeteMiejsca++;
  if (spot3_busy) zajeteMiejsca++;

  // Aktualizacja stanów diod w Blynku i konsoli tylko wtedy, gdy zmieni się stan któregokolwiek miejsca
  if (spot1_busy != lastSpot1 || spot2_busy != lastSpot2 || spot3_busy != lastSpot3) {
    Serial.println("----------------------------------------");
    Serial.println("📊 ZMIANA STANÓW NA MIEJSCACH:");
    Serial.print("   Miejsce 1: "); Serial.println(spot1_busy ? "🔴 ZAJĘTE" : "🟢 WOLNE");
    Serial.print("   Miejsce 2: "); Serial.println(spot2_busy ? "🔴 ZAJĘTE" : "🟢 WOLNE");
    Serial.print("   Miejsce 3: "); Serial.println(spot3_busy ? "🔴 ZAJĘTE" : "🟢 WOLNE");
    Serial.print("   Zajętych łącznie: "); Serial.print(zajeteMiejsca); Serial.println("/3");
    Serial.println("----------------------------------------");

    // --- STEROWANIE DIODAMI W BLYNKU ---
    // SLOT 1 (V0 = Zielona Wolne, V3 = Czerwona Zajęte)[cite: 2]
    if (spot1_busy) {
      Blynk.virtualWrite(V0, 0);   
      Blynk.virtualWrite(V3, 255); 
    } else {
      Blynk.virtualWrite(V0, 255); 
      Blynk.virtualWrite(V3, 0);   
    }

    // SLOT 2 (V1 = Zielona Wolne, V4 = Czerwona Zajęte)[cite: 2]
    if (spot2_busy) {
      Blynk.virtualWrite(V1, 0);
      Blynk.virtualWrite(V4, 255);
    } else {
      Blynk.virtualWrite(V1, 255);
      Blynk.virtualWrite(V4, 0);
    }

    // SLOT 3 (V2 = Zielona Wolne, V5 = Czerwona Zajęte)[cite: 2]
    if (spot3_busy) {
      Blynk.virtualWrite(V2, 0);
      Blynk.virtualWrite(V5, 255);
    } else {
      Blynk.virtualWrite(V2, 255);
      Blynk.virtualWrite(V5, 0);
    }

    lastSpot1 = spot1_busy;
    lastSpot2 = spot2_busy;
    lastSpot3 = spot3_busy;
  }

  // Sterowanie fizyczną czerwoną diodą na płytce (pełny parking)
  if (zajeteMiejsca >= 3) {
    digitalWrite(RED_LED_PIN, HIGH); 
  } else {
    digitalWrite(RED_LED_PIN, LOW);
  }

  // 2. LOGIKA WYJAZDU: Auto wewnątrz podjeżdża do bramy i aktywuje IR_AFTER_GATE
  if (digitalRead(IR_AFTER_GATE) == LOW && !waitingForCarToEnter && !waitingForCarToExit) {
    Serial.println("🚗 Wykryto pojazd wewnątrz (chce wyjechać). Otwieram szlaban...");
    otwórzSzlaban();
    waitingForCarToExit = true; // Czekamy aż minie czujnik zewnętrzny
    delay(1000); // Krótkie opóźnienie, żeby auto zdążyło ruszyć z czujnika wewnętrznego
  }

  // 3. ZAMKNIĘCIE PRZY WYJEŹDZIE: Gdy auto wyjedzie na zewnątrz i minie czujnik IR_BEFORE_GATE
  if (waitingForCarToExit && digitalRead(IR_BEFORE_GATE) == LOW) {
    Serial.println("🚗 Auto opuściło parking i minęło czujnik zewnętrzny. Czekam 3 sekundy i zamykam szlaban...");
    delay(3000); 
    zamknijSzlaban();
  }

  // 4. ZAMKNIĘCIE PRZY WJEŹDZIE: Gdy auto wjeżdża z zewnątrz i mija czujnik IR_AFTER_GATE
  if (waitingForCarToEnter && digitalRead(IR_AFTER_GATE) == LOW) {
    Serial.println("🚗 Auto wjechało na teren i minęło czujnik wewnętrzny. Czekam 3 sekundy i zamykam szlaban...");
    delay(3000); 
    zamknijSzlaban();
  }

  // 5. Obsługa odczytu RFID (Wjazd z zewnątrz)
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Odczyt UID karty w formacie HEX
  String uidStr = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) uidStr += "0";
    uidStr += String(mfrc522.uid.uidByte[i], HEX);
    if (i < mfrc522.uid.size - 1) uidStr += " ";
  }
  uidStr.toUpperCase();

  Serial.println("----------------------------------------");
  Serial.print("💳 Zeskanowano UID: ");
  Serial.println(uidStr);

  // A. ZARZĄDCA (Karta A9 A5 63 06 - Zawsze wjeżdża, ignoruje limit)
  if (uidStr == "A9 A5 63 06") {
    Serial.println("👑 WITAJ ZARZĄDCO! Szlaban OTWARTY (Tryb priorytetowy).");
    otwórzSzlaban();
    waitingForCarToEnter = true;
  }
  
  // B. KARTY BEZ DOSTĘPU
  else if (uidStr == "F2 72 D8 11" || uidStr == "6D 20 E0 A5") {
    Serial.println("🚫 ODMOWA DOSTĘPU! Brak uprawnień do wjazdu.");
  }
  
  // C. KARTY Z DOSTĘPEM (Weryfikacja wolnych miejsc)
  else {
    bool isAuthorized = false;
    for (int i = 0; i < totalUsers; i++) {
      if (authorizedUIDs[i] == uidStr) {
        isAuthorized = true;
        break;
      }
    }

    if (isAuthorized) {
      if (zajeteMiejsca < 3) {
        Serial.print("✅ WJAZD DOZWOLONY. Wolne miejsca: ");
        Serial.println(3 - zajeteMiejsca);
        otwórzSzlaban();
        waitingForCarToEnter = true;
      } else {
        Serial.println("❌ PARKING PEŁNY! Wszystkie 3 miejsca są zajęte. Wjazd zablokowany.");
      }
    } else {
      Serial.println("❓ Nieznana karta/brelok. Brak dostępu.");
    }
  }

  Serial.println("----------------------------------------");
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(1000);
}