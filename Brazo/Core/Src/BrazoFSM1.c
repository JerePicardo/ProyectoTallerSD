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
void FSM_Brazo(Brazo *B, evento event){
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
				B->last_pr_comm = procesar(&(B->last_rf_comm));
				UpdateBrazo(B->pos);
                break;
                }
            break;
        case STATE_ACTIVO:
        	 if (event == EVENT_NEW_DATA) {
        		 B->flag=FLAG_PROCESAR;
        	     break;
        	 }
            if (event == EVENT_EVIL_DATA) {
            	B->actual = STATE_PARK;
                B->flag=FLAG_ERROR;
                B->Error=ERR_DATA_CORRUPTED;
                break;
            }
            if (event == EVENT_PARKEAR ) {
            	B->actual = STATE_PARK;
                B->flag=FLAG_PARK;
                break;
            }
            if (event == EVENT_TIMEOUT) {
            	B->actual = STATE_PARK;
            	//B->flag=FLAG_PARK;
            	B->Error=ERR_TIMEOUT_SYNC;
                break;
            }
            break;

        case STATE_PARK:
        	B->flag=FLAG_PARK;
            if (event == EVENT_DESPERTAR) {
            	B->actual = STATE_ACTIVO;
                break;
            }
            break;
        default: break;
    }
}

/* Initialize FSM */
void FSM_Brazo_init(Brazo * B) {
    B->actual = STATE_INICIO;
}


pr_data procesar(rx_data* d){
	pr_data out = {0};

	    if(d->flag_dormir==1){
	        out.dormido = 1;
	        return out;
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
	    return out;

}



