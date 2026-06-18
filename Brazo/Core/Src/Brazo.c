

#include <Brazo.h>


void UpdateBrazo(uint8_t posicion[7]){
	PCA9685_SetServoAngle(0, posicion[0]);
	PCA9685_SetServoAngle(1, posicion[1]);
	PCA9685_SetServoAngle(2, posicion[2]);
	PCA9685_SetServoAngle(3, posicion[3]);
	PCA9685_SetServoAngle(4, posicion[4]);
	PCA9685_SetServoAngle(3, posicion[5]);
	PCA9685_SetServoAngle(4, posicion[6]);
}

void Brazo_MoveJoint(uint8_t posicion[7] , uint8_t ch, uint8_t deg){
	PCA9685_SetServoAngle(ch, deg);
	posicion[ch]=deg;
}

void Brazo_MoveCartesian(uint8_t posicion[7], uint8_t x, uint8_t y, uint8_t z){
	//mi idea era poder llevarl brazo, la pinza a un punto que vos se lo das
	//honestamente no se como hacer esta funcion jjaja
}

void park(uint8_t posicion[7]){
	posicion[0] = 0;
	posicion[1] = 75;
	posicion[2] = 15;
	posicion[3] = 180;
	posicion[4] = 180;
	posicion[5] = 0;
	posicion[6] = 0;
	UpdateBrazo(posicion);
}



