//Funcionamiento
//1. Coneccion a wifi
//2. Interaccion con un bot de Telegram
//3. Por medio del bot de Telegram, encender y apagar el Led incluido
//   en la ESP32 con comandos envidos desde la app de Telegram en celualr

//Para coneccion a WiFi
#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
const char* ssid = "INFINITUM05FC";
const char* password = "Tr9Ji9Cy6u";

//Para coneccion con Telegram
#include <UniversalTelegramBot.h>   
#include <ArduinoJson.h>
#define BOTtoken "5558241338:AAHtuW4E3YYOYWcLMmQ4TgC8iBD1eAKoN2c"  
#define CHAT_ID "5558241338"

#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);


int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

//---Utilizamos el LED que se encuentra en el pin 4---//
const int ledPin = 4;
bool ledState = LOW;

//-Funcion que nos permite enviar instrucciones a travez de la app-//
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Bienvenido, " + from_name + ".\n";
      welcome += "Utilice los siguientes comando para interactuar con el ESP32.\n\n";
      welcome += "/led_on para encender el LED \n";
      welcome += "/led_off para apagar el LED \n";
      welcome += "/state para obtener el estado del LED \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/led_on") {
      bot.sendMessage(chat_id, "LED ON", "");
      ledState = LOW;
      digitalWrite(ledPin, ledState);
    }
    
    if (text == "/led_off") {
      bot.sendMessage(chat_id, "LED OFF", "");
      ledState = HIGH;
      digitalWrite(ledPin, ledState);
    }
    
    if (text == "/state") {
      if (digitalRead(ledPin)){
        bot.sendMessage(chat_id, "LED esta apagado", "");
      }
      else{
        bot.sendMessage(chat_id, "LED esta encendido", "");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);

  #ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");      
    client.setTrustAnchors(&cert); 
  #endif

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  
  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); 
  #endif
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  Serial.println(WiFi.localIP());
}

void loop() {
  Serial.println("1");
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    Serial.println("2");
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
