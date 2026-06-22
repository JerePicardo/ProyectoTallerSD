/*
 * BrazoFSM1.c
 *
 *  Created on: 28 may 2026
 *      Author: PC-Casa
 */

#include <BrazoFSM1.h>

static rx_data rx_buffer;
static char txt[32];

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

void processComm(Brazo * B) {


	if (B->last_rf_comm.flag_dormir == 1) {
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

