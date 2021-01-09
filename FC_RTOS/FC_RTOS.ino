  /*
 * Example to demonstrate thread definition, semaphores, and thread sleep.
 */

#include <Arduino_due_FreeRTOS.h>
#include "FC_RTOS.h"


const uint8_t LEDs[6] = {13, 31, 33, 35, 37, 39};

TaskHandle_t  PidHandler          = NULL;
TaskHandle_t  ActuatorsHandler    = NULL;
TaskHandle_t  UartFromNAVCHandler = NULL;
TaskHandle_t  ErrorHandler        = NULL;

uint8_t NAVC_RxBuffer[10];

void InitBoard(void){

  /* Inicialzacion de UART */
  Serial.setTimeout(500);
  Serial.begin(115200);
  Serial2.begin(115200);

  /* Pin de interrupcion */
  pinMode(INT_PIN, INPUT);
  /* Configuro los LEDs como salida y verifico que funcionen */
  for(uint8_t i=0 ; i<sizeof(LEDs)+1 ; i++) pinMode(LEDs[i], OUTPUT);
  for(uint8_t i=0 ; i<sizeof(LEDs)+1 ; i++) digitalWrite(LEDs[i], HIGH);
  delay(500);
  for(uint8_t i=0 ; i<sizeof(LEDs)+1 ; i++) digitalWrite(LEDs[i], LOW);

  attachInterrupt(digitalPinToInterrupt(INT_PIN), NAVC_IRQHandler, FALLING);

}


static void PID(void *p) {

  TickType_t  PidLastUnblock = xTaskGetTickCount();
  //digitalWrite(LED_AXIS, HIGH);

  while(1){
    /* Controlador PID, update cada 500ms */
    vTaskDelayUntil(&PidLastUnblock, pdMS_TO_TICKS(500));
    digitalWrite(LED_UPDATE, !digitalRead(LED_UPDATE));
  }
}


static void Actuators(void *p){
  TickType_t ActLastUnblock = xTaskGetTickCount();
  //digitalWrite(LED_AXIS, HIGH);

  while(1){
    /* Actuadores con update cada 250ms */
    vTaskDelayUntil(&ActLastUnblock, pdMS_TO_TICKS(250));
  }
}


static void UartFromNAVC(void *p){
  while(1){
    vTaskSuspend(NULL);
    /* UART Rx Handler */
    //if(Serial2.available()){
      digitalWrite(LED_UART, HIGH);
      Serial2.readBytesUntil(END_CHAR, NAVC_RxBuffer, sizeof(NAVC_RxBuffer));
      digitalWrite(LED_UART, LOW);
    //}
  }
}


static void ErrorTask(void *p){
  vTaskSuspend(NULL);
  while(1){
    vTaskDelay(750);
    digitalWrite(LED_ERROR, HIGH);
    vTaskDelay(250);
    digitalWrite(LED_ERROR, LOW);
  }
}


void NAVC_IRQHandler(){
  digitalWrite(LED_AXIS, !digitalRead(LED_AXIS));
  BaseType_t CheckifYieldRequired;
  CheckifYieldRequired = xTaskResumeFromISR(UartFromNAVCHandler);
  portYIELD_FROM_ISR(CheckifYieldRequired);
}


void setup() {

  InitBoard();

  portBASE_TYPE PidTask       = xTaskCreate(PID, "PID", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, &PidHandler);
  portBASE_TYPE ActuatorTask  = xTaskCreate(Actuators, "Actuators", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+2, &ActuatorsHandler);
  portBASE_TYPE UartTask      = xTaskCreate(UartFromNAVC, "UartFromNAVC", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+3, &UartFromNAVCHandler);
  portBASE_TYPE Errortask     = xTaskCreate(ErrorTask, "Error", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, &ErrorHandler);


  // Chequeo errores en creacion de tareas
  if (PidTask != pdPASS || ActuatorTask != pdPASS || UartTask != pdPASS || Errortask != pdPASS){
    Serial.println(F("Error: Task creation problem"));
    digitalWrite(LED_ERROR, HIGH);
    while(1);
  }

  vTaskStartScheduler();

  Serial.println("Error: Insufficient RAM");
  digitalWrite(LED_ERROR, HIGH);
  while(1);
}



// Loos must never be blocked, not used.
void loop() {}
