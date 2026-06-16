

#include <Brazo.h>


void UpdateBrazo(Brazo* B){
	PCA9685_SetServoAngle(0, B->posicion[0]);
	PCA9685_SetServoAngle(1, B->posicion[1]);
	PCA9685_SetServoAngle(2, B->posicion[2]);
	PCA9685_SetServoAngle(3, B->posicion[3]);
	PCA9685_SetServoAngle(4, B->posicion[4]);
	PCA9685_SetServoAngle(3, B->posicion[5]);
	PCA9685_SetServoAngle(4, B->posicion[6]);
}

void Brazo_MoveJoint(Brazo* B , uint8_t ch, uint8_t deg){
	PCA9685_SetServoAngle(ch, deg);
	B->posicion[ch]=deg;

}

void Brazo_MoveCartesian(Brazo* B, uint8_t x, uint8_t y, uint8_t z){
	//mi idea era poder llevarl brazo, la pinza a un punto que vos se lo das
	//honestamente no se como hacer esta funcion jjaja
}

void park(Brazo * B){
	B->posicion[0] = 0;
	B->posicion[1] = 75;
	B->posicion[2] = 15;
	B->posicion[3] = 180;
	B->posicion[4] = 180;
	B->posicion[5] = 0;
	B->posicion[6] = 0;
	UpdateBrazo(B);
}



