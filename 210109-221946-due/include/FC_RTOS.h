
/*   TODO:
*    Como no esta clara la implementacion de comunicacion UART con arduino y FreeRTOS, se 
*    establecen como constantes definidas las variables que en teoria tendrian que llegar
*    via UART. 
*    Ver como resolverlo.
*/


#define LED_PIN         (uint8_t)13
#define LED_ERROR       (uint8_t)31
#define LED_UPDATE      (uint8_t)33
#define LED_UART        (uint8_t)35
#define LED_AXIS        (uint8_t)37
#define LED_GPS         (uint8_t)39

#define INT_PIN         (uint8_t)29
#define NAVC_MSG_SIZE   (uint8_t)18
#define END_CHAR        (char)'\n'


typedef union{
    
    struct{
        char        Start;              // Byte de inicio de mensaje
        float       Pitch;              // Pitch de la NAVC
        float       Roll;               // Roll de la NAVC
        float       Yaw;                // Yaw de la NAVC
        float       Option;             // Valor opcional
        char        Stop;               // Byte de stop
    };
    
    char BufferData[NAVC_MSG_SIZE];

}NAVC_data;


void NAVC_IRQHandler();



