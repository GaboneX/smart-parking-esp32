# 🚗 Smart Parking System (ESP32, RFID & Blynk IoT)

Kompleksowy, w pełni zautomatyzowany model inteligentnego parkingu na 3 stanowiska, oparty na mikrokontrolerze ESP32. Projekt łączy systemy wbudowane, bezprzewodową identyfikację radiową (RFID), automatykę bramową sterowaną czujnikami optycznymi (IR) oraz monitoring w czasie rzeczywistym w chmurze IoT.

---

## 🛠️ Wykorzystane technologie i komponenty

### 🔌 Sprzęt:
* **Mikrokontroler:** ESP32 DevKit v1
* **Czytnik RFID:** MFRC522 (komunikacja SPI)
* **Aktuator:** Serwomechanizm (sterowanie szlabanem)
* **Detekcja pojazdów:** 
  * 3x Czujniki zajętości miejsc parkingowych
  * Optyczne czujniki podczerwieni IR (kontrola wjazdu, wyjazdu oraz zamykania automatycznego)
* **Inne:** Przewody połączeniowe / płytki stykowe

### 💻 Oprogramowanie:
* **Środowisko:** Arduino IDE
* **Biblioteki C++:** `WiFi.h`, `BlynkSimpleEsp32.h`, `SPI.h`, `MFRC522.h`, `ESP32Servo.h`
* **Chmura / IoT:** Blynk IoT (Dashboard mobilny i webowy)

---

## 📌 Mapa połączeń (GPIO ESP32)

| Komponent / Peryferium | Pin ESP32 (GPIO) | Opis / Funkcja |
| :--- | :---: | :--- |
| **RFID - SDA (SS)** | GPIO 22 | Wybór urządzenia SPI (Chip Select) |
| **RFID - RST** | GPIO 25 | Pin resetowania czytnika RFID |
| **RFID - SCK / MISO / MOSI** | GPIO 18, 19, 23 | Magistrala SPI |
| **Szlaban (Serwo)** | GPIO 13 | Sterowanie ramieniem bramy |
| **Czujnik IR - Wjazd (Zewnętrzny)** | GPIO 32 | Zamykanie szlabanu przy wyjeździe |
| **Czujnik IR - Za bramą (Wewnętrzny)** | GPIO 33 | Zamykanie po wjeździe / otwarcie przy wyjeździe |
| **Czujnik Miejsca 1** | GPIO 14 | Stan pierwszego stanowiska |
| **Czujnik Miejsca 2** | GPIO 26 | Stan drugiego stanowiska |
| **Czujnik Miejsca 3** | GPIO 27 | Stan trzeciego stanowiska |

---

## ⚙️ Logika działania systemu

1. **Autoryzacja RFID (Wjazd):** 
   * Kierowca przykłada kartę do czytnika. System weryfikuje UID w bazie uprawnionych użytkowników.
   * **Karta Zarządcy (`A9 A5 63 06`):** Posiada tryb priorytetowy – zawsze otwiera szlaban, ignorując aktualny limit zajętych miejsc.
   * **Karty Standardowe:** Zezwalają na wjazd pod warunkiem, że zajętych jest mniej niż 3 miejsca. Przy pełnym parkingu wjazd jest blokowany.
2. **Automatyka bramy i bezpieczeństwo:**
   * **Wjazd:** Po autoryzacji szlaban się otwiera. Auto wjeżdża, mija czujnik wewnętrzny (`IR_AFTER_GATE`), system odczekuje 3 sekundy i automatycznie zamyka szlaban.
   * **Wyjazd:** Auto podjeżdża od środka, aktywując czujnik wewnętrzny (`IR_AFTER_GATE`), co otwiera szlaban. Po wyjeździe pojazd mija czujnik zewnętrzny (`IR_BEFORE_GATE`), system odczekuje 3 sekundy i zamyka szlaban.
3. **Monitorowanie stanowisk:**
   * Stan 3 miejsc parkingowych monitorowany jest w czasie rzeczywistym i przesyłany do chmury.

---

## ☁️ Integracja z aplikacją Blynk IoT

Projekt wykorzystuje platformę Blynk do wizualizacji stanu parkingu w czasie rzeczywistym:
* **Template ID:** `TMPL4qHCSMR3a`
* **Template Name:** `Car Parking`

### Konfiguracja w Blynk:
* **Krok 1: Tworzenie szablonu (Template)**
  <br><img width="933" height="353" alt="Template Config" src="https://github.com/user-attachments/assets/69340829-a4eb-4d44-91b8-465241d116c9" />

* **Krok 2: Konfiguracja Datastreams (Piny wirtualne)**
  <br><img width="912" height="348" alt="Datastreams" src="https://github.com/user-attachments/assets/6dc585bd-d956-4fd1-bf9d-4dc1b4e77cd7" />
  * `V0` / `V3` – Miejsce 1 (Zielona / Czerwona dioda)[cite: 2]
  * `V1` / `V4` – Miejsce 2 (Zielona / Czerwona dioda)[cite: 2]
  * `V2` / `V5` – Miejsce 3 (Zielona / Czerwona dioda)[cite: 2]

* **Krok 3: Web / Mobile Dashboard**
  <br><img width="945" height="362" alt="Dashboard" src="https://github.com/user-attachments/assets/03c05bff-092c-42a4-8b53-77732d39b9e7" />

---

## 📸 Zdjęcia układu

<img width="2880" height="2160" alt="Hardware 1" src="https://github.com/user-attachments/assets/0c3c22a4-9568-4f8d-98f5-e81c7fc7e88c" />
<img width="1213" height="945" alt="Hardware 2" src="https://github.com/user-attachments/assets/c4f302ed-987a-415d-bbe6-7f8253cdde18" />
<img width="1283" height="741" alt="Hardware 3" src="https://github.com/user-attachments/assets/70e96df1-7b0a-46ce-9b8e-b6521a6db314" />
<img width="1400" height="945" alt="Hardware 4" src="https://github.com/user-attachments/assets/ee95fe67-f8be-4e82-8fcc-80e44ece0ce6" />
