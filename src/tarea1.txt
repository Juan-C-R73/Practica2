
#include <Arduino.h>

// --- Configuración de pines ---
#define LED_PIN 2  // Configuracion del LED (Cambiar en laboratorio)

// --- Handles de las tareas (opcional, útil para depuración o borrado luego) ---
TaskHandle_t handleParpadeoLED = NULL;
TaskHandle_t handleEnvioUART   = NULL;

// --- Prototipos de las tareas ---
void TareaParpadeoLED(void *parametro);
void TareaEnvioUART(void *parametro);

void setup() {
  Serial.begin(115200); //Velocidad de conexion
  delay(1000); // da tiempo a que el monitor serie se estabilice

  pinMode(LED_PIN, OUTPUT);

  // Tarea 1: parpadea el LED cada 200 ms
  xTaskCreate(
    TareaParpadeoLED,     // función de la tarea
    "ParpadeoLED",        // nombre descriptivo
    2048,                 // tamaño de pila (words/bytes según plataforma, 2048 suele bastar)
    NULL,                 // parámetros
    1,                    // prioridad
    &handleParpadeoLED    // handle de la tarea
  );

  // Tarea 2: envía "hola mundo" por UART cada segundo
  xTaskCreate(
    TareaEnvioUART,       //igual que arriba
    "EnvioUART",
    2048,
    NULL,
    1,                    //Se puede modificar la prioridad para ver que la de mayor prio entra antes (en caso de que sean pedidas a la vez)
    &handleEnvioUART
  );

  // El scheduler de FreeRTOS ya está corriendo en el ESP32, viene integrado en el micro,
  // así que loop() puede quedar prácticamente vacío.
}

void loop() {
  // No se necesita hacer nada aquí: todo el trabajo lo hacen las tareas.
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}

// ------------------------------------------------------
// Tarea periódica: parpadea el LED cada 200 ms
// ------------------------------------------------------
void TareaParpadeoLED(void *parametro) {
  (void) parametro;

  for (;;) {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN)); // alterna el estado del LED, comprimes encender y apagar en una linea
    vTaskDelay(200 / portTICK_PERIOD_MS);          // cede el CPU durante 200 ms
  }
}

// ------------------------------------------------------
// Tarea periódica: envía "hola mundo" por UART cada segundo
// ------------------------------------------------------
void TareaEnvioUART(void *parametro) {
  (void) parametro;

  for (;;) {
    Serial.println("hola mundo");
    vTaskDelay(1000 / portTICK_PERIOD_MS); // cede el CPU durante 1000 ms
  }
}
