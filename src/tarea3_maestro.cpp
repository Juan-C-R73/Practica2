#include "Arduino.h"
#include "Wire.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
// NUEVA LIBRERÍA: Permite usar los "ganchos" en el ESP32
#include "esp_freertos_hooks.h" 

#define LED_PIN 2

struct Aceleracion {
  float x, y, z;
};

Aceleracion muestras[10];
SemaphoreHandle_t semaforoEnvio;

// ---------------------------------------------------------
// NUEVO: La función del Idle Hook (Modo bajo consumo)
// ---------------------------------------------------------
bool hookModoBajoConsumo(void) {
  // asm("waiti 0") es una instrucción directa al procesador físico.
  // Pone el procesador en estado de suspensión (Sleep) deteniendo el reloj.
  // El núcleo se despertará automáticamente en el siguiente milisegundo 
  // cuando FreeRTOS lance su "Tick" interno del reloj.
  //Comprobará si hay alguna tarea que tenga que despertarse 
  asm("waiti 0"); 
  
  return true; // Le decimos a FreeRTOS que todo ha ido bien
}

// ---------------------------------------------------------
// TAREA 1: Muestreo 
// ---------------------------------------------------------
void TareaMuestreo(void *parameter) {
  int contador = 0;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(100); 

  for(;;) {
    Wire.requestFrom(8, sizeof(Aceleracion)); 
    if (Wire.available() == sizeof(Aceleracion)) {
      Wire.readBytes((byte*)&muestras[contador], sizeof(Aceleracion));
      contador++;
    }

    if (contador >= 10) {
      contador = 0;
      xSemaphoreGive(semaforoEnvio);
    }
    vTaskDelayUntil(&xLastWakeTime, xFrequency); 
  }
}

// ---------------------------------------------------------
// TAREA 2: Envío UART y LED 
// ---------------------------------------------------------
void TareaUART_LED(void *parameter) {
  for(;;) {
    if (xSemaphoreTake(semaforoEnvio, portMAX_DELAY) == pdTRUE) {
      Serial.println("\n--- PAQUETE ESP32 (1 Segundo) ---");
      for (int i = 0; i < 10; i++) {
        Serial.printf("T+%dms -> AccX: %.2f | AccY: %.2f | AccZ: %.2f\n", 
                      (i+1)*100, muestras[i].x, muestras[i].y, muestras[i].z);
      }
      
      digitalWrite(LED_PIN, HIGH);
      vTaskDelay(pdMS_TO_TICKS(200)); 
      digitalWrite(LED_PIN, LOW);
    }
  }
}

// ---------------------------------------------------------
// CONFIGURACIÓN INICIAL
// ---------------------------------------------------------
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  Wire.begin(); 

  semaforoEnvio = xSemaphoreCreateBinary();

  // NUEVO: Registramos nuestro gancho en el sistema operativo ANTES de crear tareas
  esp_register_freertos_idle_hook(hookModoBajoConsumo);

  xTaskCreate(TareaMuestreo, "Muestreo", 2048, NULL, 1, NULL);
  xTaskCreate(TareaUART_LED, "UART_LED", 2048, NULL, 1, NULL);
}

void loop() {
 
}