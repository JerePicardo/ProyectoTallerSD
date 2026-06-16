/*
 * BrazoFSM1.h
 *
 *  Created on: 28 may 2026
 *      Author: PC-Casa
 */

#ifndef SRC_BRAZOFSM1_H_
#define SRC_BRAZOFSM1_H_


#include <stdint.h>

typedef enum {
	FLAG_IDLE,
	FLAG_PROCESAR,
	FLAG_PARK,
	FLAG_ERROR,
	FLAG_TIMEOUT
}flag_t ;

typedef struct{
    int16_t acelerometros[2][3];
    int16_t giroscopios[2][3];
    uint8_t pote;
    uint8_t boton;
    uint8_t secuencia;
    uint8_t flag_dormir;
} data;
typedef struct{
    uint8_t servo[7];
    uint8_t error;
    uint8_t dormido;
} data_pr;

typedef enum{
    ERR_NONE = 0,
	ERR_TIMEOUT_SYNC,
    ERR_NRF_SYNC_LOST,
    ERR_PCA9685_NOT_FOUND,
	ERR_DATA_CORRUPTED
} ErrorCode;

/* States */
typedef enum {
    STATE_INICIO,
    STATE_ACTIVO,
    STATE_ERROR,
    STATE_PARK
} estado;

/* Events */
typedef enum {
    EVENT_NEW_DATA,
	EVENT_TICK,
	EVENT_EVIL_DATA,
    EVENT_TIMEOUT,
    EVENT_PARKEAR,
    EVENT_DESPERTAR
} evento;

typedef struct{
	data dato;
	data_pr info;
	uint8_t posicion[5];
	flag_t flag;
	ErrorCode Error;
}Brazo;


estado FSM_Brazo(estado current, evento event, Brazo* B);
estado FSM_Brazo_init(Brazo * B);
data_pr procesar(data* d);


#endif /* SRC_BRAZOFSM1_H_ */
