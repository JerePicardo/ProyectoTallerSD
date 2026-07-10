/*
 * BrazoFSM1.c
 *
 *  Created on: 28 may 2026
 *      Author: PC-Casa
 */



#include <BrazoFSM1.h>

/* Process event and execute actions */
estado FSM_Brazo(estado current, evento event,Brazo* B){
    estado next = current;
    switch (current) {
        case STATE_INICIO:
            if (event == EVENT_READY) {
            	//todas las estructuras necesarias fueron inicializadas
                next = STATE_SINC;
                break;
            }
            // tendreia que hacer algun caso de error o mensaje algo en cas de que falle
            break;


        case STATE_SINC:
        	if (event == EVENT_DATA) {
        		next = STATE_PROCESAR;
        		break;
        	}
            if (event == EVENT_ERR_SINC) {
                next = STATE_ERROR;
                break;
            }
            if (event == EVENT_RX_FAIL) {
                next = STATE_PARK;
                break;
            }
            break;
        case STATE_PROCESAR:
        	 if (event == EVENT_PROCESSED) {
        		 next = STATE_SINC;
        	     break;
        	 }
            if (event == EVENT_ERR_SEN) {
                next = STATE_ERROR;
                break;
            }
            if (event == EVENT_DORMIR ) {
                next = STATE_PARK;
                break;
            }
            if (event == EVENT_ERR_TX) {
            	next = STATE_ERROR;
                break;
            }
            break;
        case STATE_ERROR:
            if (event == EVENT_READY) {
                next = STATE_SINC;
                break;
            }
            break;
        case STATE_PARK:
            if (event == EVENT_DESPERTAR) {
                next = STATE_SINC;
                break;
            }
            break;
        default: break;
    }

    if (next != current) {
        switch (next) {
            case STATE_SINC:
                //ESPERAR MENSAJE DE SINCRONZACION PRINEAR EN PANTALLA;
                break;
            default: break;
        }
    }
    return next;
}

/* Initialize FSM */
estado FSM_Brazo_init(Brazo * B) {
    estado initial = STATE_INICIO;
    *B = (Brazo){0};
    park(B);
    return initial;
}

void park(Brazo * B){
	  B->posicion[0] = 180;
	  B->posicion[1] = 180;
	  B->posicion[2] = 180;
	  B->posicion[3] = 180;
	  B->posicion[4] = 180;
}

data procesar(data* d){
	data out = {0};

	    if(d->flag_dormir==1){
	        out.flag_dormir = 1;
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



