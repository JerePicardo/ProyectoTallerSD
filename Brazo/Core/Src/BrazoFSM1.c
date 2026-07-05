/*
 * BrazoFSM1.c
 *
 *  Created on: 28 may 2026
 *      Author: PC-Casa
 */

#include <BrazoFSM1.h>
#include <math.h>
#include <stdint.h>

static rx_data rx_buffer;
static char txt[32];
static void processComm(Brazo *B);
static float mapf(float x, float in_min, float in_max, float out_min, float out_max);
static float clampf(float v, float min, float max);
static uint32_t last_ms = 0;



/* Process event and execute actions */
void FSM_Brazo(Brazo *B, evento event) {
	switch (B->actual) {
	case STATE_INICIO:
		if (event == EVENT_NEW_DATA) {
			B->actual = STATE_ACTIVO;

			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
			nrf24_receive((uint8_t*) &rx_buffer, sizeof(rx_data));
			// Print para debug
			sprintf(txt, "AX:%d", rx_buffer.acelerometros[0][0]);
			mensaje_ssd(txt, Font_6x8, 0, 0, 1);
			sprintf(txt, "Ay:%d", rx_buffer.acelerometros[0][1]);
			mensaje_ssd(txt, Font_6x8, 0, 1, 0);
			sprintf(txt, "Az:%d", rx_buffer.acelerometros[0][2]);
			mensaje_ssd(txt, Font_6x8, 0, 2, 0);
			mensaje_ssd("Sincronizado", Font_6x8, 0, 3, 0);
			//HAL_Delay(100);

			B->last_rf_comm = rx_buffer;
			processComm(B);
			UpdateBrazo(B->pos);
		}
		break;
	case STATE_ACTIVO:
		switch (event) {
		case EVENT_NEW_DATA:
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
			nrf24_receive((uint8_t*) &rx_buffer, sizeof(rx_data));
			// Print para debug
			sprintf(txt, "AX:%d", rx_buffer.acelerometros[0][0]);
			mensaje_ssd(txt, Font_6x8, 0, 0, 1);
			sprintf(txt, "Ay:%d", rx_buffer.acelerometros[0][1]);
			mensaje_ssd(txt, Font_6x8, 0, 1, 0);
			sprintf(txt, "Az:%d", rx_buffer.acelerometros[0][2]);
			mensaje_ssd(txt, Font_6x8, 0, 2, 0);
			mensaje_ssd("Sincronizado", Font_6x8, 0, 3, 0);
			//HAL_Delay(100);

			B->last_rf_comm = rx_buffer;
			processComm(B);
			UpdateBrazo(B->pos);
			break;
		case EVENT_TIMEOUT:
		case EVENT_PARKEAR:
			mensaje_ssd("parkeado bien chill", Font_6x8, 0, 0, 1);
			// HAL_TIM_Base_Start_IT(&htim1);
			park(B->pos);
			break;
		case EVENT_EVIL_DATA:
			mensaje_ssd("err: datos corruptos", Font_6x8, 0, 0, 1);
			break;
		default:
			break;
	}
	break;

	case STATE_PARK:
		if (event == EVENT_NEW_DATA) {
			B->actual = STATE_ACTIVO;

			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
			nrf24_receive((uint8_t*) &rx_buffer, sizeof(rx_data));
			// Print para debug
			sprintf(txt, "AX:%d", rx_buffer.acelerometros[0][0]);
			mensaje_ssd(txt, Font_6x8, 0, 0, 1);
			sprintf(txt, "Ay:%d", rx_buffer.acelerometros[0][1]);
			mensaje_ssd(txt, Font_6x8, 0, 1, 0);
			sprintf(txt, "Az:%d", rx_buffer.acelerometros[0][2]);
			mensaje_ssd(txt, Font_6x8, 0, 2, 0);
			mensaje_ssd("Sincronizado", Font_6x8, 0, 3, 0);
			//HAL_Delay(100);

			B->last_rf_comm = rx_buffer;
			processComm(B);
			UpdateBrazo(B->pos);
			break;
		}
		break;
	default:
		break;
	}
}

/* Initialize FSM */
void FSM_Brazo_init(Brazo *B) {
	B->actual = STATE_INICIO;
}

#define RAD_TO_DEG      57.2957795f
#define GYRO_SENS_250   131.0f      // LSB/(°/s) para ±250°/s
#define ALPHA           0.98f       // filtro complementario

static float clampf(float v, float min, float max){
    if (v < min) return min;
    if (v > max) return max;
    return v;
}

static float mapf(float x, float in_min, float in_max, float out_min, float out_max){
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static void processComm(Brazo *B)
{
    static float roll_f[2]  = {0.0f, 0.0f};
    static float pitch_f[2] = {0.0f, 0.0f};

    if(!last_ms){
        last_ms = B->last_rf_comm.timeStamp;
    	return;
    }

    float dt = ((B->last_rf_comm.timeStamp - last_ms) / 1000.0f);
    last_ms = B->last_rf_comm.timeStamp;

    for (int s = 0; s < 2; s++)
    {
        float ax = (float)B->last_rf_comm.acelerometros[s][0];
        float ay = (float)B->last_rf_comm.acelerometros[s][1];
        float az = (float)B->last_rf_comm.acelerometros[s][2];

        float gx = (float)B->last_rf_comm.giroscopios[s][0] / GYRO_SENS_250;
        float gy = (float)B->last_rf_comm.giroscopios[s][1] / GYRO_SENS_250;

        float roll_acc  = atan2f(ay, az) * RAD_TO_DEG;
        float pitch_acc = atan2f(-ax, sqrtf(ay * ay + az * az)) * RAD_TO_DEG;

        roll_f[s]  = ALPHA * (roll_f[s]  + gx * dt) + (1.0f - ALPHA) * roll_acc;
        pitch_f[s] = ALPHA * (pitch_f[s] + gy * dt) + (1.0f - ALPHA) * pitch_acc;
    }

    /*
      OJO: estos rangos son de ejemplo.
      Hay que ajustarlos según el recorrido mecánico real de cada servo.
      Si pos[] no guarda grados sino PWM, cambia esta parte.
    */

    B->pos[0] = (int32_t)clampf(mapf(roll_f[0],  -45.0f,  45.0f,   0.0f, 180.0f), 0.0f, 180.0f);  // hombro: abducción
    B->pos[1] = (int32_t)clampf(mapf(pitch_f[0], -45.0f,  45.0f,   0.0f, 180.0f), 0.0f, 180.0f);  // hombro: flex/ext
    B->pos[2] = (int32_t)clampf(mapf(roll_f[1],  -90.0f,  90.0f,   0.0f, 180.0f), 0.0f, 180.0f);  // muñeca: giro
    B->pos[3] = (int32_t)clampf(mapf(pitch_f[1], -90.0f,  90.0f,   0.0f, 180.0f), 0.0f, 180.0f);  // muñeca: flexión

    if (B->last_rf_comm.flag == FLAG_DORMIR) {
		B->last_pr_comm.dormido = 1;
	}
	/*
	 if(!ComprobarRangos(d))
	 {
	 out.error = 1;
	 return out;
	 }

	 ConvertirUnidades(d);

	 FiltrarDatos(d);

	 CalcularOrientacion(d, &out);

	 CalcularServos(d, &out);
	 */
}
