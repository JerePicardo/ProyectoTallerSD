/*
 * BrazoFSM1.h
 *
 *  Created on: 28 may 2026
 *      Author: PC-Casa
 */

#ifndef SRC_BRAZOFSM1_H_
#define SRC_BRAZOFSM1_H_


#include <stdint.h>

typedef struct{
    int16_t acelerometros[2][3];
    int16_t giroscopios[2][3];
    uint8_t pote;
    uint8_t boton;
    uint8_t secuencia;
    uint8_t flag_dormir;
} data;
typedef struct{
    float pitch;
    float roll;
    uint8_t servo[5];
    uint8_t error;
    uint8_t dormido;
} data_procesada;

typedef enum{
    ERR_NONE = 0,
	ERR_TIMEOUT_SYNC,
    ERR_NRF_SYNC_LOST,
    ERR_PCA9685_NOT_FOUND,
    ERR_ADC_TIMEOUT,
    ERR_SERVO_OUT_OF_RANGE,
	ERR_DATA_CORRUPTED
} ErrorCode;

typedef struct{
	data dato;
	uint8_t posicion[5];
	uint8_t flag_error;
	uint8_t flag_sinc;
	uint8_t flag_dormir;
	uint8_t flag_desperar;
	ErrorCode Error;
}Brazo;

/* States */
typedef enum {
    STATE_INICIO,
    STATE_SINC,
    STATE_PROCESAR,
    STATE_ERROR,
    STATE_PARK
} estado;

/* Events */
typedef enum {
    EVENT_READY,
    EVENT_DATA,
    EVENT_ERR_SINC,
    EVENT_ERR_SEN,
    EVENT_ERR_TX,
    EVENT_DORMIR,
    EVENT_DESPERTAR,
    EVENT_PROCESSED,
    EVENT_RX_FAIL
} evento;

/*typedef enum {
	 FLAG_SINC,
	 FLAG_DORMIR,
	 FLAG_IRQ,
	 FLAG_DESPERTAR,
	 FLAG_ERROR,
	 FLAG_IDLE
}flag ;
*/

estado FSM_Brazo(estado current, evento event, Brazo* B);
estado FSM_Brazo_init(Brazo * B);
data procesar(data* d);
void park(Brazo * B);

#endif /* SRC_BRAZOFSM1_H_ */
