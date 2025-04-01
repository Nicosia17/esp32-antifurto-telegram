# ESP32 Antifurto con Telegram

**IT 🇮🇹**  
Sistema antifurto IoT basato su ESP32 che utilizza un sensore PIR per rilevare intrusioni. Il sistema invia notifiche via Telegram agli utenti autorizzati e consente la gestione remota tramite pulsanti inline. Include anche un sistema di log con timestamp reali.

**EN 🇬🇧**  
IoT anti-theft system based on ESP32 using a PIR sensor to detect intrusions. The system sends real-time notifications via Telegram to authorized users and allows remote control via inline buttons. Includes a logging system with real timestamps.

---

## 🔧 Componenti utilizzati / Components used

- ESP32
- Sensore PIR
- Connessione Wi-Fi
- Bot Telegram
- NTP server per orario reale

---

## 🛠️ Setup

1. Inserisci il tuo SSID e la password Wi-Fi nel codice (`main.cpp`)
2. Crea un bot su Telegram con [BotFather](https://t.me/botfather) e copia il token
3. Inserisci gli ID Telegram autorizzati
4. Carica il codice sull’ESP32 usando PlatformIO o Arduino IDE

---

## 📷 Funzionalità principali / Key Features

- Rilevazione movimento con PIR
- Notifiche Telegram automatiche
- Comandi Telegram con pulsanti inline
- Log con timestamp reali
- Attivazione/disattivazione da remoto

---

## 📩 Comandi Telegram disponibili

- `/start` – Avvia il bot e mostra i comandi disponibili
- Pulsanti: 🔒 Attiva | 🔓 Disattiva | 📋 Mostra log | 🧹 Cancella log

---

## 🧠 Autore

Progetto sviluppato da **Nicosia Salvatore** 
📧 *nicosiasalvatore17@gmail.com*
