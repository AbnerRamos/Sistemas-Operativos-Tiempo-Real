//#include <timers.h>

#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Variables globales
static TimerHandle_t one_shot_timer = NULL;

// defina el led de su eleccion
static const int led_pin = 2;

//definimos una variable para almacenar lo que se lee en la uart
char entrada; 

//*****************************************************************************
// Callbacks

// La función es llamada cuando el timer expira
void myTimerCallback(TimerHandle_t xTimer) {

  // Mensaje del timer 0
  if ((uint32_t)pvTimerGetTimerID(xTimer) == 0) {
    Serial.println("One-shot timer expired");
    digitalWrite(led_pin, HIGH); //se enciende el led
  }else{
    digitalWrite(led_pin, LOW);  //se apaga el led
  }
}

//*****************************************************************************
// Main (runs as its own task with priority 1 on core 1)

void setup() {

   // Configure Serial
   Serial.begin(115200);

  // configure LED
  pinMode(led_pin,OUTPUT);
 
  // retardo 
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS Timers---");

  // Timer Simple tiro
  one_shot_timer = xTimerCreate(
                      "One-shot timer",           // Name of timer
                      50000 / portTICK_PERIOD_MS,  // Period of timer (in ticks)
                      pdFALSE,                    // Auto-reload
                      (void *)0,                  // Timer ID
                      myTimerCallback);           // Callback function

  // Garantizamos que fue creado
  if (one_shot_timer == NULL) {
    Serial.println("Could not create one of the timers");
  } else {
    
    // mensaje inicial 
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.println("Starting timers...");
    Serial.println(" Introducir 1 para encender el LED: ");
    // Start timers
    xTimerStart(one_shot_timer, portMAX_DELAY);
  }

  // Delete self task to show that timers will work with no user tasks
  vTaskDelete(NULL);
}


void loop() {
  entrada = Serial.parseInt(); //leemos el puerto serial
  if (entrada == 1){           //si el valor que se lee es 1, entonces encendemos el timer
    xTimerStart(one_shot_timer, portMAX_DELAY);
  }
}
