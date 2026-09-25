#include <Arduino.h>
#include <Wire.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#define LED_PIN 2 // LED integrado del ESP32

struct Aceleracion {
  float x, y, z;
};

Aceleracion muestras[10];
SemaphoreHandle_t semaforoEnvio;

// ---------------------------------------------------------
// TAREA 1: Muestreo (Pide datos al Nano cada 100 ms)
// ---------------------------------------------------------
void TareaMuestreo(void *parameter) {
  int contador = 0;
  TickType_t xLastWakeTime = xTaskGetTickCount(); //tiempo base para calcular el siguiente despertar
  const TickType_t xFrequency = pdMS_TO_TICKS(100); //100 ms a ticks del procesador

  for(;;) {
    // Le pedimos la caja (12 bytes) a la placa esclava número #8
    Wire.requestFrom(8, sizeof(Aceleracion)); 
    
    // Si la recibimos bien, la guardamos en nuestro array
    if (Wire.available() == sizeof(Aceleracion)) {
      Wire.readBytes((byte*)&muestras[contador], sizeof(Aceleracion));
      contador++;
    }

    if (contador >= 10) {
      contador = 0;
      xSemaphoreGive(semaforoEnvio); // Entregamos el testigo a la otra tarea
    }

    vTaskDelayUntil(&xLastWakeTime, xFrequency); // Dormimos la tarea 100ms desde la última vez que se despertó
  }
}

// ---------------------------------------------------------
// TAREA 2: Envío UART y LED
// ---------------------------------------------------------
void TareaUART_LED(void *parameter) {
  for(;;) {
    // Esperamos durmiendo hasta que la Tarea 1 nos de el semáforo
    if (xSemaphoreTake(semaforoEnvio, portMAX_DELAY) == pdTRUE) {
      
      Serial.println("\n--- PAQUETE ESP32 (1 Segundo) ---");
      for (int i = 0; i < 10; i++) {
        Serial.printf("T+%dms -> AccX: %.2f | AccY: %.2f | AccZ: %.2f\n", 
                      (i+1)*100, muestras[i].x, muestras[i].y, muestras[i].z);
      }
      
      digitalWrite(LED_PIN, HIGH);
      vTaskDelay(pdMS_TO_TICKS(200)); // tarea que se duerma 200ms
      digitalWrite(LED_PIN, LOW);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  Wire.begin(); // Iniciamos el ESP32 como Maestro I2C

  semaforoEnvio = xSemaphoreCreateBinary();

  xTaskCreate(TareaMuestreo, "Muestreo", 2048, NULL, 1, NULL);
  xTaskCreate(TareaUART_LED, "UART_LED", 2048, NULL, 1, NULL);
}

void loop() {}