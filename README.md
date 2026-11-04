# Práctica 2 

Este repositorio contiene las soluciones para la Práctica 2.

## 🚀 Tareas implementadas

*   **Tarea 1 (`tarea1_2.txt`):** Creación de un programa básico con dos tareas independientes ejecutándose en el ESP32. La primera tarea hace parpadear un LED cada 200 ms, mientras que la segunda envía el mensaje "hola mundo" por la UART cada segundo.
*   **Tarea 2 (`tarea2_maestro.txt` y `tarea2_esclavo.txt`):** Integración de un sensor inercial mediante comunicación I2C en una arquitectura Maestro-Esclavo.
    *   El esclavo lee continuamente la aceleración en los ejes X, Y, Z y envía una estructura de 12 bytes cuando se le solicita.
    *   El maestro (ESP32) utiliza dos tareas sincronizadas por un semáforo binario. Una tarea muestrea los datos del sensor cada 100 ms. Cuando acumula 10 muestras (1 segundo), cede el semáforo a la segunda tarea, la cual envía el paquete completo por UART y enciende un LED durante 200 ms.
*   **Tarea 3 (`tarea3_maestro.cpp`):** Optimización energética mediante el modo de bajo consumo de FreeRTOS. Se implementa un *Idle Hook* (`esp_freertos_idle_hook`) que ejecuta la instrucción en ensamblador `asm("waiti 0")` para suspender el procesador cuando no hay tareas listas para ejecutarse, despertando automáticamente en el siguiente ciclo de reloj.

## 🛠️ Tecnologías y Librerías Utilizadas

*   **Plataformas:** ESP32 (Maestro) y Arduino (Esclavo con IMU integrado).
*   **Sistema Operativo:** FreeRTOS integrado en el entorno de Arduino para ESP32.
*   **Librerías principales:**
    *   `freertos/FreeRTOS.h`, `freertos/task.h`, `freertos/semphr.h`: Para la gestión de tareas, tiempos y semáforos.
    *   `esp_freertos_hooks.h`: Para registrar la función de gancho de bajo consumo en el ESP32.
    *   `Wire.h`: Para la comunicación I2C.
    *   `Arduino_LSM9DS1.h`: Para la lectura del sensor inercial de la placa esclava.
