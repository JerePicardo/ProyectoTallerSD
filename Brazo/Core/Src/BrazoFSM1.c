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
            if (event == EVENT_NEW_DATA) {
                next = STATE_ACTIVO;
                B->flag=FLAG_IDLE;
                break;
                }
            break;
        case STATE_ACTIVO:
        	 if (event == EVENT_NEW_DATA) {
        		 next = STATE_ACTIVO;
        		 B->flag=FLAG_PROCESAR;
        	     break;
        	 }
            if (event == EVENT_EVIL_DATA) {
                next = STATE_PARK;
                B->flag=FLAG_ERROR;
                B->Error=ERR_DATA_CORRUPTED;
                break;
            }
            if (event == EVENT_PARKEAR ) {
                next = STATE_PARK;
                B->flag=FLAG_PARK;
                break;
            }
            if (event == EVENT_TIMEOUT) {
            	next = STATE_PARK;
            	//B->flag=FLAG_PARK;
            	B->Error=ERR_TIMEOUT_SYNC;
                break;
            }
            break;

        case STATE_PARK:
        	B->flag=FLAG_PARK;
            if (event == EVENT_DESPERTAR) {
                next = STATE_ACTIVO;
                break;
            }
            break;
        default: break;
    }

    return next;
}

/* Initialize FSM */
estado FSM_Brazo_init(Brazo * B) {
    estado initial = STATE_INICIO;
    *B = (Brazo){0};
    return initial;
}


data_pr procesar(data* d){
	data_pr out = {0};

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



