/*
 * Brazo.c
 *
 *  Created on: 4 jun 2026
 *      Author: PC-Casa
 */

#include <Brazo.h>


void UpdateBrazo(Brazo* B){
	PCA9685_SetServoAngle(0, B->posicion[0]);//HAY CANAL 0??
	PCA9685_SetServoAngle(1, B->posicion[1]);
	PCA9685_SetServoAngle(2, B->posicion[2]);
	PCA9685_SetServoAngle(3, B->posicion[3]);
	PCA9685_SetServoAngle(4, B->posicion[4]);
}

void Brazo_MoveJoint(Brazo* B , uint8_t ch, uint8_t deg){
	PCA9685_SetServoAngle(ch, deg);
	 B->posicion[ch]=deg;

}

void Brazo_MoveCartesian(Brazo* B, uint8_t x, uint8_t y, uint8_t z){
	//mi idea era poder llevarl brazo, la pinza a un punto que vos se lo das
	//honestamente no se como hacer esta funcion jjaja
}

void Brazo_Stop(Brazo* B){
	park(B);
	UpdateBrazo(B);
	// no se si hacer otra cosa o simplemente llamar a park cuando haga fala y updatear
	//lo pongo por si se me ocurre otra funcion que ocupe el lugar de esta
	// a chat le parecio buena idea a mi no tanto
}
