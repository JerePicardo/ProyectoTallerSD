/*
 * BrazoFSM1.h
 *
 *  Created on: 28 may 2026
 *      Author: PC-Casa
 */

#ifndef SRC_BRAZOFSM1_H_
#define SRC_BRAZOFSM1_H_


#include <stdint.h>
#include <stdio.h>

#include "main.h"
#include "Brazo.h"
#include "NRF24.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"

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
} rx_data;

typedef struct{
    uint8_t servo[7];
    uint8_t error;
    uint8_t dormido;
} pr_data;

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
	estado actual;
	uint8_t pos[7];
	flag_t flag;
	ErrorCode Error;
	rx_data last_rf_comm;
	pr_data last_pr_comm;
} Brazo;


void FSM_Brazo(Brazo * B, evento event);
void FSM_Brazo_init(Brazo * B);
pr_data procesar(rx_data* d);


#endif /* SRC_BRAZOFSM1_H_ */
