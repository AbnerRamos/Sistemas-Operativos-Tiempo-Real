//Funcionamiento
//utilizar la comunicación Bluethoot de la ESP32, al precionar un boton, se 
//habilita la comunicación, permaneciendo disponible hasta que se conecta un 
//dispositivo, una vez conectado, muestra en el monitor serie los mensajes
//enviados y se desconecta con un comando especifico enviado por bluethoot

//-----Agregamos libreria necesaria-----//
#include <BluetoothSerial.h>
#define LED1_GPIO 5

BluetoothSerial SerialBT;
//-----------Para el boton---------------//
int btn =14; 
bool control = true;
int estado = 0;

//----------Definimos la tara-----------//
void ConfigbyBluetooth (void);
 
void setup() {
//-------Para coneccion bluethoot------//
  pinMode(LED1_GPIO, OUTPUT);
  pinMode(btn, INPUT);
  pinMode(4, OUTPUT);
//-Para coneccion con el monitor serie-//
  Serial.begin(115200);
  Serial.println("inicio de codigo");
  digitalWrite(4,HIGH);
}

void loop() {
//-logica de precionar boton, evitando el ruido mecánico-//
  control = true;
  if (btn == 0 && control == true && estado == 0){
    delay (20);
    estado = 1;
  }
  if (btn == 0 && control == true && estado == 1){
    estado = 2;
    digitalWrite(4, HIGH);
  }
  if (estado == 2 && btn == 1){
    ConfigbyBluetooth();
  }
}

//-------Funcion de bluethoot--------//
void ConfigbyBluetooth(void){
  String message = "";
  String message2 = "";
  char incomingChar;
  char incomingChar2;
  bool BTHControl = true;
  SerialBT.begin("Sis.op.real"); // <-- nombre de ESP32 en bluethoot
  Serial.println("Bluetooth disponible");
  while(BTHControl==true){
    if(SerialBT.available()){
      char incomingChar = SerialBT.read();
      if (incomingChar != '\n'){
        message += String(incomingChar);
      }
      
      else{
        message ="";
      }
      
      Serial.write(incomingChar);
    } 
//-desconección de bluethoot por medio del comando Reset-//
    if (message == "Reset"){
      BTHControl = false;
      control = false;
    }
    
    if(Serial.available()){
      char incomingChar2 = Serial.read();
      if(incomingChar2 != '\n'){
        message2 += String(incomingChar2);
      }
      else{
        message2 = "";
      }
      Serial.write(incomingChar2);
    }
  }
  SerialBT.end();
}
