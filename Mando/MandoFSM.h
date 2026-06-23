


#ifndef ManndoFSM_h
#define ManndoFSM_h

#include <Arduino.h>
#include <Hardware.h>
typedef enum{
ESTADO_ACTIVE,
ESTADO_SLEEP,
ESTADO_MANUAL,
ESTADO_ERROR
} estado;

typedef enum{
EVENTO_NONE,
EVENTO_SAMPLE,
EVENTO_SLEEP_TIMEOUT,
EVENTO_WAKEUP,
EVENTO_BUTTON_PRESS,
EVENTO_MANUAL_CMD,
EVENTO_EXIT_MANUAL,
EVENTO_RF_TIMEOUT,
EVENTO_SENSOR_TIMEOUT,
EVENTO_BAT_LOW,
EVENTO_ERROR
}evento;

typedef struct {
  uint16_t acelerometros[2][3];
  uint16_t giroscopios[2][3];
  uint8_t pote;
  uint8_t boton;
  estado st_actual;
  evento ev_actual;
} Mando;

evento updateEvents(Mando *);
estado MandoFSM(Mando * M);

//transmitir un mensaje 
//FSM_update
//FSM Innit
//Printear Datos

#endif