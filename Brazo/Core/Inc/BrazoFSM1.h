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

#define  FLAG_EMPTY           (1 << 0)
#define  FLAG_DORMIR          (1 << 1)
#define  FLAG_MANUAL          (1 << 2)
#define  FLAG_APRETAR_PINZA   (1 << 3)
#define RAD_TO_DEG      57.2957795f
#define GYRO_SENS_250   131.0f      // LSB/(°/s) para ±250°/s
#define ALPHA           0.70f       // filtro complementario
#define POTE_0			0			// límite inferior potenciómetro codo
#define POTE_180		255			// límite superior potenciómetro codo

#define LIM_SUP_SERVO_0 	170
#define LIM_SUP_SERVO_1 	170
#define LIM_SUP_SERVO_2 	170
#define LIM_SUP_SERVO_3 	170
#define LIM_SUP_SERVO_4 	170
#define LIM_SUP_SERVO_5 	170
#define LIM_SUP_SERVO_6 	170

#define LIM_INF_SERVO_0 	10
#define LIM_INF_SERVO_1 	10
#define LIM_INF_SERVO_2 	10
#define LIM_INF_SERVO_3 	10
#define LIM_INF_SERVO_4 	10
#define LIM_INF_SERVO_5 	10
#define LIM_INF_SERVO_6 	10

#define OFFSET_SERVO_0 	0
#define OFFSET_SERVO_1 	1
#define OFFSET_SERVO_2 	2
#define OFFSET_SERVO_3 	3
#define OFFSET_SERVO_4 	4
#define OFFSET_SERVO_5 	5
#define OFFSET_SERVO_6 	6


typedef struct {
  int16_t acelerometros[2][3];
  int16_t giroscopios[2][3];
  uint8_t pote;
  uint16_t timeStamp;
  uint8_t angle;
  uint8_t channel;
  uint8_t flag;
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
    STATE_PARK,
	STATE_MANUAL
} estado;

/* Events */
typedef enum {
    EVENT_NEW_DATA,
	EVENT_TICK,
	EVENT_EVIL_DATA,
    EVENT_TIMEOUT,
    EVENT_PARKEAR,
    EVENT_DESPERTAR,
	EVENT_MANUAL,
	EVENT_AUTO,
	EVENT_MAS,
	EVENT_MENOS
} evento;

typedef struct{
	estado actual;
	uint8_t pos[7];
	ErrorCode Error;
	rx_data last_rf_comm;
	pr_data last_pr_comm;
} Brazo;


void FSM_Brazo(Brazo * B, evento event);
void FSM_Brazo_init(Brazo * B, UART_HandleTypeDef *huartf);


#endif /* SRC_BRAZOFSM1_H_ */
