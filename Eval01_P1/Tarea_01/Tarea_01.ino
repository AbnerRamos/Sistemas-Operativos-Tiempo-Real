//-------para conexion wifi---------//
#include <WiFi.h>
#include <HTTPClient.h>
#define WIFI_SSID "INFINITUM05FC"
#define WIFI_PASSWORD "Tr9Ji9Cy6u"
//----------------------------------//

//--------para Ultrasonix----------//
#include <time.h>
//---------------------------------//

//------Para FireBase--------------//
#include <Firebase_ESP_Client.h>
//Provide the token generation process info.
#include <addons/TokenHelper.h>
//Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>
//For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino
/* 2. Define the API Key */
#define API_KEY "AIzaSyCU2gEaDatp4H_EjEc42gngvvEVqMbI1RI"
/* 3. Define the RTDB URL */
#define DATABASE_URL "https://esp32upiizrtos-default-rtdb.firebaseio.com/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app
/* 4. Define the user Email and password that alreadey registerd or added in your project */
//serial bluethoot terminal

#define USER_EMAIL "abnerr018@gmail.com"
#define USER_PASSWORD "123456789"
//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
unsigned long count = 0;
//---------------------------------//

//------Cnexion wifi----//
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;
String fecha = "0";
int control = 0;
//----------------------//

//------Ultrasonix------//
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701 
const int trigPin = 14;
const int echoPin = 2;
long duration;
float distanceOn;
float distanceInch;
// ---------------------//

//Definimos las tareas
void GetDate(void);
void Ultrasonix(void);
void FireBase(void);

void setup() {
//Para coneccion a wifi
  Serial.begin(9600);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting");
  while(WiFi.status()!=WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  control = 1;
  Serial.println(" Conection successful");
  Serial.print("Conectando con la siguiente dirección IP: ");
  Serial.println(WiFi.localIP());
//Para ultrasonico
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin,LOW);
//Para Firebase
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  /* Assign the api key (required) */
  config.api_key = API_KEY;
  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;
  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  //Or use legacy authenticate metho
  //config.database_url = DATABASE_URL;
  //config.signer.tokens.legacy_token = "<database secret>";
  Firebase.begin(&config, &auth);
  //Comment or pass false value when WiFi reconnection will control by your code or third party library
  Firebase.reconnectWiFi(true);
}

void loop() {
  while(1){
    GetDate();
    Ultrasonix();
    FireBase();
    delay(5000);
  }

}

void GetDate (void){
  struct tm timeinfo;
  //Int and get the date
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  
  char timeYear[5];
  strftime(timeYear,5,"%Y", &timeinfo);
  char timeMonth[15];
  strftime(timeMonth,15,"%B", &timeinfo);
  char timeDay[10];
  strftime(timeDay,10,"%d", &timeinfo);
  char timeHour[3];
  strftime(timeHour,3,"%H", &timeinfo);
  char timeMin[3];
  strftime(timeMin,3,"%M", &timeinfo);
  fecha = String(timeYear)+String(timeMonth)+String(timeDay)+String("  ")+String(timeHour)+String(":")+String(timeMin);
  Serial.println(fecha);
}

void Ultrasonix(void){
  //limpiar el trigger
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  //poner el trigger en alto 10 microsegundos
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  //leer el pin echo
  duration = pulseIn(echoPin, HIGH);

  //calcular la distancias
  distanceOn = duration * SOUND_SPEED/2;

  //Imprimimos la distancia
  Serial.print("Disance (cm): ");
  Serial.println(distanceOn);
}

void FireBase(void){
    //Flash string (PROGMEM and  (FPSTR), String C/C++ string, const char, char array, string literal are supported
    //in all Firebase and FirebaseJson functions, unless F() macro is not supported.

    if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
    {
        sendDataPrevMillis = millis();

        FirebaseJson FECHA;
        FECHA.add("Fecha", fecha);
        Serial.printf("Set fecha... %s\n", Firebase.RTDB.setJSON(&fbdo, "/Tarea01/Fecha", &FECHA) ? "ok" : fbdo.errorReason().c_str());

        FirebaseJson longitud;
        longitud.setFloatDigits(4);
        longitud.add("Distancia", distanceOn);
        Serial.printf("Set Distancia... %s\n", Firebase.RTDB.setJSON(&fbdo, "/Tarea01/Distancia", &longitud) ? "ok" : fbdo.errorReason().c_str());
    }
}
