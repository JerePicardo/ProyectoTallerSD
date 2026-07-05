#ifndef ManndoFSM_h
#define ManndoFSM_h

#include <Arduino.h>
#include "Hardware.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wire.h>
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
EVENTO_MANUAL_CMD,        //
EVENTO_EXIT_MANUAL,       // SOLO HACE FALTA UNO?
EVENTO_RF_TIMEOUT,        //hACE FALTA TANTOS TIMEOUTS? SE PUEDE ENCAPSULAR EN 1
EVENTO_SENSOR_TIMEOUT,
EVENTO_BAT_LOW,           // NO SE HACERLO
EVENTO_ERROR        
}evento;

typedef enum{
  FLAG_DORMIR,
  FLAG_MANUAL,
  FLAG_APRETAR_PINZA
} FLAG_t;

typedef struct {
  uint16_t acelerometros[2][3];
  uint16_t giroscopios[2][3];
  uint8_t pote;
  uint8_t boton;
  uint16_t TIMESTAMP;
  uint8_t angle;
  uint8_t ch;
  FLAG_t f ;
}data;

typedef struct {
  data payload;
  estado state;
  evento event;
} Mando;

typedef struct
{
    evento buffer[EVENT_QUEUE_SIZE];
    volatile uint8_t head;
    volatile uint8_t tail;
} EventQueue;

extern EventQueue eventQueue;

bool pushEvent(evento ev);
evento popEvent(void);
evento updateEvents(void);
estado MandoFSM(Mando * M);

//transmitir un mensaje 
//FSM_update
//FSM Innit
//Printear Datos

#endif