// * Dos tareas se encargan de controlar el toggle de un GPIO
// * El toggle de una tarea deberá ser del doble de la frecuencia de la otra tarea.

//Configuramos los nucleos que se van a utilizar
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else 
static const BaseType_t app_cpu =1;
#endif

//Utilizaremos los GPIO 14 y 15 para e led Amarillo y rojo respectivamente
 int led_pin1 =4; //Led Amarillo
static const int led_pin2 = 15; //Led Rojo

//necesitamos un toggle que sea el doble del otro 
static const int rate_1 = 600; //ms
static const int rate_2 = 300; //ms


//Tarea para led Rojo con toggle de 600ms
void toggleLED_1(void *parameter) {
  while(1) {
    digitalWrite(led_pin2, HIGH);
    vTaskDelay(rate_1 / portTICK_PERIOD_MS);
    digitalWrite(led_pin2, LOW);
    vTaskDelay(rate_1 / portTICK_PERIOD_MS);
  }
}

//Tarea para led Amarillo con toggle de 300ms
void toggleLED_2(void *parameter) {
  while(1) {
    digitalWrite(led_pin1, HIGH);
    vTaskDelay(rate_2 / portTICK_PERIOD_MS);
    digitalWrite(led_pin1, LOW);
    vTaskDelay(rate_2 / portTICK_PERIOD_MS);
  }
}


void setup() {  
  pinMode(led_pin1, OUTPUT);
  pinMode(led_pin2, OUTPUT);

//llamamos la tarea del Rojo
  xTaskCreatePinnedToCore(  
              toggleLED_1,  
              "Toggle 1",   
              1024,         
              NULL,         
              1,            
              NULL,         
              app_cpu);
                   
//llamamos la tarea del Amarillo
  xTaskCreatePinnedToCore(  
              toggleLED_2,  
              "Toggle 2",   
              1024,         
              NULL,         
              2,            
              NULL,         
              app_cpu);     
}

void loop() {
}
// * Cuando ambas tareas se ejcutan con la misma prioridad vemos como repentinamente una 
//   accion se empala con la otra
//
// * Cuando damos prioridad al led rojo, este no se empalma ni interrumpe, pero el amarillo sí
//
// * Caso contrario cuando priorisamos el led amarillo, el rojo es el que se interrumpe de 
//   vez en cuando
