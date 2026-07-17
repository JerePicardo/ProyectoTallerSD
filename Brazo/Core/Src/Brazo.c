#include <Brazo.h>

uint8_t selected_servo = 0;

void changeManualServo(uint8_t new_servo){
	selected_servo = new_servo;
}

void UpdateBrazo(uint8_t posicion[7]){
	PCA9685_SetServoAngle(0, posicion[0]);
	PCA9685_SetServoAngle(1, posicion[1]);
	PCA9685_SetServoAngle(2, posicion[2]);
	PCA9685_SetServoAngle(3, posicion[3]);
	PCA9685_SetServoAngle(4, posicion[4]);
	PCA9685_SetServoAngle(5, posicion[5]);
	PCA9685_SetServoAngle(6, posicion[6]);
}

void Brazo_MoveJoint(uint8_t posicion[7] , uint8_t ch, uint8_t deg){
	PCA9685_SetServoAngle(ch, deg);
	posicion[ch]=deg;
}

void Brazo_MoveCartesian(uint8_t posicion[7], uint8_t x, uint8_t y, uint8_t z){
	//mi idea era poder llevarl brazo, la pinza a un punto que vos se lo das
	//honestamente no se como hacer esta funcion jjaja

	//Nota de Jere:
	//tenes q definir un ssistema de coordenadas para hacer esto. Y hay que tener en cuenta
	//que el brazo trabajaría en algo bastante más complejo que coordenadas esféricas dada
	//la articulación del codo, así que esta función es un bardo bárbaro de implementar.
	//veremos si de la filtración de kalman se pueden sacar datos de posición absoluta viables.
}

void park(uint8_t posicion[7]){
	posicion[0] = 180;
	posicion[1] = 75;
	posicion[2] = 0;
	posicion[3] = 180;
	posicion[4] = 90;
	posicion[5] = 45;
	posicion[6] = 30;
	UpdateBrazo(posicion);
}

uint8_t Brazo_getSelectedServo(){
	return selected_servo;
}

void Brazo_increaseAngle(uint8_t pos[7]){
	if(pos[selected_servo] >= 180)
		return;

	pos[selected_servo]++;
	PCA9685_SetServoAngle(selected_servo, pos[selected_servo]);
}

void Brazo_decreaseAngle(uint8_t pos[7]){
	if(pos[selected_servo] <= 0)
		return;

	pos[selected_servo]--;
	PCA9685_SetServoAngle(selected_servo, pos[selected_servo]);
}


