#include <WiFi.h>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>
#include <vector>
#include <time.h>

// ======= CREDENZIALI WiFi E BOT =======
const char* ssid = "TUO_SSID"; // Inserisci il nome della tua rete Wi-Fi
const char* password = "TUA_PASSWORD_WIFI"; // Inserisci la password della tua rete
#define BOT_TOKEN "IL_TUO_TOKEN_BOT" // Inserisci il token del tuo bot Telegram

WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// ======= ID AUTORIZZATI =======
const String ID_UTENTE_1 = "INSERISCI_ID_TELEGRAM_1";
const String ID_UTENTE_2 = "INSERISCI_ID_TELEGRAM_2";

// ======= PIN SENSORE PIR =======
#define PIR_PIN 14

// ======= VARIABILI DI STATO =======
bool sistemaAttivo = false;
unsigned long ultimoTempoRilevazione = 0;
const unsigned long intervalloRilevazione = 2000;
std::vector<String> logAllarmi;

// ======= ORARIO REALE (NTP) =======
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

String getTimestamp() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "Orario non disponibile";
  char buffer[30];
  strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", &timeinfo);
  return String(buffer);
}

// ======= INVIA MESSAGGIO FORMATTATO =======
void inviaMessaggio(String messaggio) {
  String formattato = "🔔 <b>NOTIFICA ANTIFURTO</b>\n\n" + messaggio;
  bot.sendMessage(ID_UTENTE_1, formattato, "HTML");
  bot.sendMessage(ID_UTENTE_2, formattato, "HTML");
}

// ======= PULSANTI INLINE TELEGRAM =======
void inviaPulsanti(String chatId) {
  String keyboardJson = R"([
    [
      {"text": "🔒 Attiva sistema", "callback_data": "attiva"},
      {"text": "🔓 Disattiva sistema", "callback_data": "disattiva"}
    ],
    [
      {"text": "📋 Mostra log", "callback_data": "mostra_log"},
      {"text": "🧹 Cancella log", "callback_data": "cancella_log"}
    ]
  ])";
  bot.sendMessageWithInlineKeyboard(chatId, "Seleziona un'azione:", "", keyboardJson);
}

// ======= GESTIONE COMANDI E CALLBACK =======
void gestisciComandi() {
  int nuoviMessaggi = bot.getUpdates(bot.last_message_received + 1);
  for (int i = 0; i < nuoviMessaggi; i++) {
    String tipo = bot.messages[i].type;
    String chatId = bot.messages[i].chat_id;
    String testo = bot.messages[i].text;

    if (chatId != ID_UTENTE_1 && chatId != ID_UTENTE_2) {
      bot.sendMessage(chatId, "🚫 Non sei autorizzato ad usare questo bot.", "");
      continue;
    }

    if (tipo == "callback_query") {
      String queryData = testo;
      if (queryData == "attiva") {
        if (!sistemaAttivo) {
          sistemaAttivo = true;
          bot.sendMessage(chatId, "🟢 <b>Sistema attivato!</b>\nAttesa di 10 secondi prima dell'inizio delle rilevazioni...", "HTML");
          delay(10000);
          bot.sendMessage(chatId, "🎯 Sistema pronto. Il sensore PIR è ora in ascolto.", "HTML");
        } else {
          bot.sendMessage(chatId, "⚠️ Il sistema è già attivo.", "");
        }
      }
      else if (queryData == "disattiva") {
        sistemaAttivo = false;
        bot.sendMessage(chatId, "🔕 <b>Sistema disattivato!</b>", "HTML");
      }
      else if (queryData == "mostra_log") {
        if (logAllarmi.empty()) {
          bot.sendMessage(chatId, "📭 Nessun allarme registrato al momento.", "");
        } else {
          String messaggio = "📋 <b>LOG ALLARMI</b>\n\n";
          for (auto& entry : logAllarmi) messaggio += "• " + entry + "\n";
          bot.sendMessage(chatId, messaggio, "HTML");
        }
      }
      else if (queryData == "cancella_log") {
        logAllarmi.clear();
        bot.sendMessage(chatId, "🧹 Log cancellato correttamente!", "");
      }
    }
    else if (testo == "/start") {
      String nomeUtente = (chatId == ID_UTENTE_1) ? "Utente 1" : "Utente 2";
      String messaggio = "👋 <b>Ciao " + nomeUtente + "</b>, benvenuto nel sistema antifurto!\n\nUsa i pulsanti qui sotto per gestire il sistema.";
      bot.sendMessage(chatId, messaggio, "HTML");
      inviaPulsanti(chatId);
    }
  }
}

// ======= CONNESSIONE WiFi E ORARIO =======
void connettiWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n✅ Connesso al WiFi!");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

// ======= SETUP =======
void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  connettiWiFi();
  client.setInsecure();
  bot.sendMessage(ID_UTENTE_1, "🚨 <b>Sistema antifurto avviato</b>!\n\nPremi /start per visualizzare i comandi.", "HTML");
  bot.sendMessage(ID_UTENTE_2, "🚨 <b>Sistema antifurto avviato</b>!\n\nPremi /start per visualizzare i comandi.", "HTML");
}

// ======= LOOP =======
void loop() {
  gestisciComandi();
  if (sistemaAttivo && (millis() - ultimoTempoRilevazione > intervalloRilevazione)) {
    ultimoTempoRilevazione = millis();
    if (digitalRead(PIR_PIN)) {
      String log = "⚠️ Movimento rilevato il " + getTimestamp();
      logAllarmi.push_back(log);
      inviaMessaggio("<b>" + log + "</b>");
    }
  }
}

