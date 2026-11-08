# smart-parking-esp32
System automatycznego parkingu opartego na ESP32 z obsługą RFID, czujnikami IR i aplikacją Blynk

1. Opis projektu
Projekt polega na budowie zintegrowanego systemu parkingowego wykorzystującego:
•	mikrokontroler ESP32
•	czujniki IR podczerwieni do detekcji pojazdów
•	serwomechanizm do sterowania bramą wjazdową/wyjazdową
•	aplikację mobilną Blynk do prezentacji danych i wizualizacji stanu miejsc parkingowych.
System w czasie rzeczywistym wykrywa zajętość miejsc parkingowych, automatycznie otwiera/zamyka bramę oraz informuje użytkownika o stanie parkingu poprzez aplikację
2. Zastosowane komponenty
 Sprzęt:
Element	Ilość
ESP32 DevKit v1	1
Czujniki IR	5
Serwomechanizm SG92R	1
Przewody połączeniowe	20
Płytka stykowa	1
 Oprogramowanie:
•	Arduino IDE z bibliotekami:
o	BlynkSimpleEsp32.h
o	ESP32Servo.h
•	Aplikacja Blynk IoT
________________________________________
3. Schemat podłączenia
Element	GPIO ESP32	Opis
Czujnik miejsca 1	D2	SENSOR_1_PIN
Czujnik miejsca 2	D4	SENSOR_2_PIN
Czujnik miejsca 3	D5	SENSOR_3_PIN
Czujnik przed bramą	D18	GATE_SENSOR_1_PIN
Czujnik za bramą	D19	GATE_SENSOR_2_PIN
Serwo	D21	SERVO_PIN

5. Integracja z aplikacją Blynk
 Krok 1: Utworzenie Template w Blynk
<img width="933" height="353" alt="image" src="https://github.com/user-attachments/assets/69340829-a4eb-4d44-91b8-465241d116c9" />
•	Template ID: TMPL4LEQxtsOg
•	Template Name: ESP32
•	Device: ESP32 Dev Board
•	Template settings: ESP32, WiFi

Krok 2: Konfiguracja Datastreams
<img width="912" height="348" alt="image" src="https://github.com/user-attachments/assets/6dc585bd-d956-4fd1-bf9d-4dc1b4e77cd7" />

W Datastreams utworzono następujące pola:
Nazwa	Pin Wirtualny	Typ	Opis
Miejsce 1 Zielony	V0	Digital	LED - miejsce wolne
Miejsce 1 Czerwony	V1	Digital	LED - miejsce zajęte
Miejsce 2 Zielony	V2	Digital	LED - miejsce wolne
Miejsce 2 Czerwony	V3	Digital	LED - miejsce zajęte
Miejsce 3 Zielony	V4	Digital	LED - miejsce wolne
Miejsce 3 Czerwony	V5	Digital	LED - miejsce zajęte

Krok 3: Web Dashboard
<img width="945" height="362" alt="image" src="https://github.com/user-attachments/assets/03c05bff-092c-42a4-8b53-77732d39b9e7" />
•	6 LED widgetów (zielony/czerwony dla każdego miejsca)

5. Opis działania systemu
Monitorowanie miejsc parkingowych:
•	System sprawdza stany wejściowe z 3 czujników IR.
•	Dla każdego czujnika:
o	Jeśli nie wykrywa przeszkody → miejsce wolne (zielona dioda)
o	Jeśli wykrywa przeszkodę → miejsce zajęte (czerwona dioda)
•	Zliczana jest liczba zajętych miejsc.
•	Status jest wysyłany do Blynk (V0–V5).
Zarządzanie bramą:
•	Jeśli czujnik przed bramą wykryje pojazd i są wolne miejsca, brama otwiera się (serwo na 90°).
•	Po przejechaniu przez bramę (czujnik za bramą), brama zamyka się automatycznie.
•	Jeżeli parking jest pełny – brama nie otwiera się.

7. Zdjęcia projektu
   <img width="2880" height="2160" alt="IMG_3239" src="https://github.com/user-attachments/assets/0c3c22a4-9568-4f8d-98f5-e81c7fc7e88c" />
   <img width="1213" height="945" alt="image" src="https://github.com/user-attachments/assets/c4f302ed-987a-415d-bbe6-7f8253cdde18" />
   <img width="1283" height="741" alt="image" src="https://github.com/user-attachments/assets/70e96df1-7b0a-46ce-9b8e-b6521a6db314" />
   <img width="1400" height="945" alt="image" src="https://github.com/user-attachments/assets/ee95fe67-f8be-4e82-8fcc-80e44ece0ce6" />







