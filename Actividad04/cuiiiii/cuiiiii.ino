// Use only core 1 for demo purposes
#if CONFIG_FREERTOS_UNICORE
  static const BaseType_t app_cpu = 0;
#else
  static const BaseType_t app_cpu = 1;
#endif

// Settings
static const uint8_t msg_queue_len = 5; //longitud de variable gobal

// Globals
static QueueHandle_t msg_queue;

//*****************************************************************************
// Tareas

void printMessages(void *parameters) {

  int item;

  // Loop infinito
  while (1) {
    
    if (xQueueReceive(msg_queue, (void *)&item, 0) == pdTRUE) {
    }
    Serial.println(item);

    // tiempo de espera para volver a imprimir
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

// Main 
void setup() {

  // Configuracion del serial
  Serial.begin(115200);
  vTaskDelay(1000 / portTICK_PERIOD_MS); //timepo de espera para empezar
  Serial.println();
  Serial.println("---FreeRTOS Queue Demo---");

  msg_queue = xQueueCreate(msg_queue_len, sizeof(int));

  // Start print task
  xTaskCreatePinnedToCore(printMessages,
                          "Print Messages",
                          1024,
                          NULL,
                          1,
                          NULL,
                          app_cpu);
}

void loop() {

  static int num  = 0;
  if (xQueueSend(msg_queue, (void *)&num, 10) != pdTRUE) {
    Serial.println("Queue full");
  }
  num++;
  vTaskDelay(1000 / portTICK_PERIOD_MS); //tiempo en el que se refresca el valor
}

//Cuando el tiempo que espera para volver a imprimir es mas chico que el tiempo de 
//refresco, imprime el mismo valor varias veces hasta que se refresca
//
//Si el tiempo en el que se refresca es mas chico que el que espera para imprimir el
//queue se llenará hasta agotar su capacidad porque no da tiempo de sacar el valor 
//antes de que lo imprima, y una vez imprime, permite aumentar el valor hasta que nuevamente
//se agote su capacidad
